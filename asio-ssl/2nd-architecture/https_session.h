#pragma once
#include "https_core.h"
#include "https_request.h"
#include "https_response.h"
#include <map>

#define BIND_1(callback) std::bind(&Session::callback, this, std::placeholders::_1)
#define BIND_2(callback) std::bind(&Session::callback, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_3(callback) std::bind(&Session::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

class Session : public std::enable_shared_from_this<Session>
{
private:
  using errorCode = boost::system::error_code;

public:
  // consturct, destructor
  Session(asio::io_context &ctx)
      : ctx_(ctx), socket_(ctx, sslCtx_), resolver_(ctx)
  {
  }

  ~Session() {}

  void send(const std::string &method, const std::string &server, const std::string &path)
  {
    HttpsRequest *request = new HttpsRequest(requestId_);
    request->initRequest(method, server, path);
    requestMap_.insert(std::pair<unsigned, HttpsRequest *>(requestId_++, request));

    tcp::resolver::query query(server, "https");
    resolver_.async_resolve(query, BIND_2(onResolve));
    // delete request;
  }

private:
  void onResolve(const errorCode &error, tcp::resolver::iterator endpointIterator)
  {
    if (error)
    {
      std::cerr << "Resolve failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Resolve done\n";

    socket_.set_verify_mode(ssl::verify_none);
    socket_.set_verify_callback(BIND_2(onVerifyCert));

    connect(endpointIterator);
  }

  bool onVerifyCert(bool preverified, ssl::verify_context &ctx)
  {
    char subjectName[256];
    X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subjectName, 256);
    std::cout << "Verifying " << subjectName << "\n";
    return preverified;
  }

  void connect(tcp::resolver::iterator endpointIterator)
  {
    asio::async_connect(
        socket_.lowest_layer(), endpointIterator, BIND_1(onConnect));
  }

  void onConnect(const errorCode &error)
  {
    if (error)
    {
      std::cerr << "Connect failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Connect done\n";
    handshake();
  }

  void handshake()
  {
    socket_.async_handshake(ssl::stream_base::client, BIND_1(onHandshake));
  }

  void onHandshake(const errorCode &error)
  {
    if (error)
    {
      std::cerr << "Handshake failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Handshake done\n";
    processRequest();
  }

  void processRequest()
  {
    // check, there are requests to send
    for (auto request : requestMap_)
    {
      asio::async_write(socket_,
                        asio::buffer(request.second->data(), request.second->size()),
                        std::bind(&Session::onSend, this, std::placeholders::_1, std::placeholders::_2, request.first));
      // delete request here ?
    }
  }

  void onSend(const errorCode &error, std::size_t bytes_transferred, unsigned reqNo)
  {
    if (error)
    {
      std::cerr << "Send failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Send done(sent bytes: " << bytes_transferred << ")\n";
    receive(receive_type::status_code, reqNo);
  }

  void receive(receive_type type, unsigned reqNo)
  {
    switch (type)
    {
    case receive_type::status_code:
    {
      asio::async_read_until(socket_, responseStreambuf_, "\r\n",
                             std::bind(&Session::onReceive, this, std::placeholders::_1, receive_type::status_code, reqNo));
      break;
    }
    case receive_type::header:
    {
      asio::async_read_until(socket_, responseStreambuf_, "\r\n\r\n",
                             std::bind(&Session::onReceive, this, std::placeholders::_1, receive_type::header, reqNo));
      break;
    }
    case receive_type::body:
    {
      asio::async_read(socket_, responseStreambuf_, asio::transfer_at_least(1),
                       std::bind(&Session::onReceive, this, std::placeholders::_1, receive_type::body, reqNo));
      break;
    }
    default:
    {
      break;
    }
    }
  }

  void onReceive(const errorCode &error, receive_type type, unsigned reqNo)
  {
    if (!error)
    {
      switch (type)
      {
      case receive_type::status_code:
      {
        std::istream stream(&responseStreambuf_);
        std::string httpVersion;
        stream >> httpVersion;

        unsigned statusCode;
        stream >> statusCode;

        std::string statusMessage;

        std::getline(stream, statusMessage);

        if (!stream || httpVersion.substr(0, 5) != "HTTP/")
        {
          std::cerr << "Invalid response\n";
          return;
        }

        if (statusCode != 200)
        {
          std::cerr << "Response returned with status code " << statusCode << "\n";
          return;
        }

        // std::cout << "Status code: " << statusCode << "\n";

        requestMap_[reqNo]->setResponseStatusCode(statusCode);
        consumeStreamBuf();
        receive(receive_type::header, reqNo);
        break;
      }
      case receive_type::header:
      {
        // memcpy?
        std::istream stream{&responseStreambuf_};
        std::string header, temp;

        while (std::getline(stream, temp) && temp != "\r")
        {
          std::cout << temp << "\n";
          header.append(temp);
          temp.clear();
        }

        requestMap_[reqNo]->setResponseHeader(header);
        consumeStreamBuf();
        receive(receive_type::body, reqNo);
        break;
      }
      case receive_type::body:
      {
        // std::cout << &responseStreambuf_;
        receive(receive_type::body, reqNo);
        break;
      }
      default:
      {
        break;
      }
      }
    }
    else if(error == asio::error::eof) 
    {
      if(type == receive_type::body) {
        requestMap_[reqNo]->setResponseBody(extractResponseData());
        consumeStreamBuf();
      }
    }
    else if (error != asio::error::eof)
    {
      std::cerr << "Receive failed (0: status_code, 1: header, 2: body) [ type: " << type << ", message: " << error.message() << " ]\n";
      return;
    }
  }

  std::string extractResponseData()
  {
    auto buffer = responseStreambuf_.data();

    auto first = asio::buffer_cast<const char *>(buffer);
    auto size = asio::buffer_size(buffer);

    return std::string(first, size);
  }

  void consumeStreamBuf() {
    auto size = asio::buffer_size(responseStreambuf_.data());
    responseStreambuf_.consume(size);
  }

private:
  asio::io_context &ctx_;
  ssl::context sslCtx_{ssl::context::sslv23};
  tcp::resolver resolver_;

  ssl::stream<tcp::socket> socket_;

  HttpsRequest *request_;
  asio::streambuf responseStreambuf_;

  unsigned sessionId = -1;
  unsigned requestId_ = 0;
  std::map<unsigned, HttpsRequest *> requestMap_;
};