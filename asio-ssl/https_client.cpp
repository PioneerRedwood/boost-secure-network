#include "https_core.h"

#include <vector>
#include <thread>

#define BIND_1(callback) std::bind(&https_client::callback, this, std::placeholders::_1)
#define BIND_2(callback) std::bind(&https_client::callback, this, std::placeholders::_1, std::placeholders::_2)

class https_client
{
private:
  using error_code = boost::system::error_code;

public:
  https_client(asio::io_context &ctx, ssl::context &ssl_ctx)
      : ctx_(ctx)
      , socket_(ctx, ssl_ctx)
      , resolver_(ctx)
      , request_strand_(ctx)
      , response_strand_(ctx)
  {
  }

  void start()
  {
  }

  // send https request
  // GET, POST, PUT, DELETE ...
  void send(const std::string &method, const std::string &server, const std::string &path)
  {
    std::cout << "send request: " << method << ' ' << server << path << '\n';

    // make request
    std::ostream request_stream{&request_};
    request_stream << method << ' ';
    request_stream << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    tcp::resolver::query query(server, "https");
    resolver_.async_resolve(query, BIND_2(on_resolve));
  }

private:
  void on_resolve(const error_code &error, tcp::resolver::iterator endpoint_iterator)
  {
    if (error)
    {
      std::cerr << "Resolve failed: " << error.message() << "\n";
      return;
    }
    std::cout << "Resolve OK\n";

    socket_.set_verify_mode(ssl::verify_none);
    // socket_.set_verify_callback(BIND_1(verify_certificate));
    socket_.set_verify_callback(BIND_2(verify_certificate));

    connect(endpoint_iterator);
  }

  bool verify_certificate(bool preverified, ssl::verify_context &ctx)
  {

    char subject_name[256];
    X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";

    return preverified;
  }

  void connect(tcp::resolver::iterator endpoint_iterator)
  {
    asio::async_connect(socket_.lowest_layer(), endpoint_iterator, BIND_1(on_connect));
  }

  void on_connect(const error_code &error)
  {
    if (error)
    {
      std::cerr << "Connect failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Connect OK\n";
    handshake();
  }

  void handshake()
  {
    socket_.async_handshake(ssl::stream_base::client, BIND_1(on_handshake));
  }

  void on_handshake(const error_code &error)
  {
    if (error)
    {
      std::cerr << "Handshake faield: " << error.message() << "\n";
      return;
    }

    std::cout << "Handshake OK\n";
    const char *header = asio::buffer_cast<const char *>(request_.data());
    std::cout << "Request: " << header << "\n";

    // strand test
    asio::async_write(socket_, request_, 
      request_strand_.wrap(BIND_2(on_send)));
  }

  void on_send(const error_code &error, std::size_t bytes_transferred)
  {
    if (error)
    {
      std::cerr << "Write failed: " << error.message() << "\n";
      return;
    }

    std::cout << "Send OK(sent: " << bytes_transferred << ")\n";
    receive(receive_type::status_code);
  }

  void receive(receive_type type)
  {
    switch (type)
    {
    case receive_type::status_code:
    {
      // strand test
      asio::async_read_until(socket_, response_, "\r\n", 
        response_strand_.wrap(std::bind(&https_client::on_receive, this, std::placeholders::_1, receive_type::status_code)));
      break;
    }
    case receive_type::header:
    {
      asio::async_read_until(socket_, response_, "\r\n\r\n",
        response_strand_.wrap(std::bind(&https_client::on_receive, this, std::placeholders::_1, receive_type::header)));
      break;
    }
    case receive_type::body:
    {
      asio::async_read(socket_, response_, asio::transfer_at_least(1),
        response_strand_.wrap(std::bind(&https_client::on_receive, this, std::placeholders::_1, receive_type::body)));
      break;
    }
    default:
    {
      break;
    }
    }
  }

  void on_receive(const error_code &error, receive_type type)
  {
    std::cout << "Receive callback.. " << type << "\n";
    if (error)
    {
      std::cerr << "Receive failed - type(0: status_code, 1: header, 2: body): " << type << ", message: " << error.message() << "\n";
      return;
    }

    switch (type)
    {
    case receive_type::status_code:
    {
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;

      unsigned status_code;
      response_stream >> status_code;

      std::string status_message;

      std::getline(response_stream, status_message);

      if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        std::cerr << "Invalid response\n";
        return;
      }

      if (status_code != 200)
      {
        std::cerr << "Response returned with status code " << status_code << "\n";
        return;
      }

      std::cout << "Status code: " << status_code << "\n";
      
      receive(receive_type::header);
      break;
    }
    case receive_type::header:
    {
      std::istream response_stream{&response_};
      std::string header;

      while (std::getline(response_stream, header) && header != "\r")
      {
        std::cout << header << "\n";
      }
      std::cout << "\n";

      if (response_.size() > 0)
      {
        std::cout << &response_;
      }

      receive(receive_type::body);
      break;
    }
    case receive_type::body:
    {
      std::cout << &response_;
      receive(receive_type::body);
      break;
    }
    default:
    {
      break;
    }
    }
  }

private:
  asio::io_context &ctx_; // for strand
  ssl::stream<tcp::socket> socket_;
  tcp::resolver resolver_;
  asio::streambuf request_;
  asio::streambuf response_;

  // we can use a strand for here
  // send multiple request
  asio::io_context::strand request_strand_;
  asio::io_context::strand response_strand_;
};

int main(int argc, char *argv[])
{
  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: ./https_client <server> <path>\n";
      return 1;
    }

    asio::io_context ctx;
    ssl::context ssl_ctx(ssl::context::sslv23);
    ssl_ctx.set_default_verify_paths();

    https_client client(ctx, ssl_ctx);

    // make a thread pool
    std::vector<std::thread> threads;

    for(int i = 0; i < 4; ++i) {
      threads.push_back(std::thread([&]{
        client.send("GET", argv[1], argv[2]);
      }));

      threads[i].join();
    }

    // concurrency

    ctx.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}