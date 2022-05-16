#pragma once
#include "https_core.h"
#include "https_request.h"

class HttpsRequest;

class HttpsResponse {
private:
  using errorCode = boost::system::error_code;
public:
  // constructor
  HttpsResponse() {}

  // destructor
  ~HttpsResponse() {}

  int getStatusCode() {
    return statusCode_;
  }
  
  // header
  // const std::vector<std::string>& getHeader() {
  //   return headers_;
  // }

  // body
  // const std::string& getBody() {
  //   return body_;
  // }

  void setRequest(HttpsRequest* request) {
    request_ = request;
  }

  void setStatusCode(int code) {
    std::cout << "code: " << code << "\n";
    statusCode_ = code;
  }

  void setHeader(const std::string& header) {
    // std::cout << "header: " << header << "\n";
    // headers_.push_back(header);
    std::cout << "header size: " << header.size() << "\n";
    header_ = header;
  }

  void setBody(const std::string& body) {
    std::cout << "body size: " << body.size() << "\n";
    body_ = body;
  }

private:
  HttpsRequest* request_;

  int statusCode_;
  // std::vector<std::string> headers_;
  std::string header_;
  std::string body_;
};