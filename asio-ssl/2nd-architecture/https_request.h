#pragma once
#include "https_core.h"
#include "https_response.h"

class HttpsResponse;

class HttpsRequest {
public:
  // constructor
  HttpsRequest(int id)
  : assignedId_(id)
  {
  }

  // destructor
  ~HttpsRequest() {}

  void initRequest(const std::string& method, const std::string& server, const std::string& path) {
    std::ostream stream{&streambuf_};

    method_ = method;
    server_ = server;
    path_ = path;
    
    stream << method_ << ' ';
    stream << path_ << " HTTP/1.0\r\n";
    stream << "Host: " << server_ << "\r\n";
    stream << "Accept: */*\r\n";
    stream << "Connection: close\r\n\r\n";

    isInitialized_ = true;

    response_ = new HttpsResponse();
    response_->setRequest(this);
  }

  bool isInitialized() {
    return isInitialized_;
  }

  const char* data() {
    const char* data = asio::buffer_cast<const char*>(streambuf_.data());
    return data;
  }

  const std::size_t size() {
    return streambuf_.size();
  }

  void setResponseStatusCode(int code) {
    response_->setStatusCode(code);
  }

  void setResponseHeader(const std::string& header) {
    response_->setHeader(header);
  }

  void setResponseBody(const std::string& body) {
    response_->setBody(body);
  }

private:
  bool isInitialized_ = false;
  unsigned assignedId_ = -1;

  std::string method_;
  std::string server_;
  std::string path_;
  asio::streambuf streambuf_;

  HttpsResponse* response_;
};

