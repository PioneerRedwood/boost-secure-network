#pragma once
#include "https_core.h"
#include "https_session.h"
#include "https_request.h"
#include "https_response.h"

#include <map>
#include <mutex> // for request concurrency

class HttpsClient {
public:
  static HttpsClient& getInstance() {
    static HttpsClient instance;
    return instance;
  }

  HttpsClient(HttpsClient&) = delete;
  void operator=(HttpsClient const&) = delete;
  
  // desructor
  ~HttpsClient() {}

  // add request
  void addRequest(const std::string& method, const std::string& server, const std::string& path) {
    // add request to request queue
    // std::shared_ptr<HttpsRequest> request = std::make_shared<HttpsRequest>(currentId_++);
    // request.get()->initRequest(method, server, path);
    std::shared_ptr<Session> session = std::make_shared<Session>(ctx_);
    session.get()->send(method, server, path);
    
    sessionMap_.insert(std::pair<unsigned, std::shared_ptr<Session>(currentId_++, session));
    processRequest();
  }
  
  // get last request
  std::unique_ptr<HttpsRequest> getLastRequest() {}
  
  // get last response
  std::unique_ptr<HttpsResponse> getLastResponse() {}

  // show(print) all request, response
  void printResponseList() {

  }

private:
  // constructor
  HttpsClient() {}

  // process request 
  void processRequest() {
    // using mutex for here?
    std::lock_guard<std::mutex> guard(requestMutex_);
    for(auto sessionPair : sessionMap_) {
      
    }
  }

  // get, post 
  void handleGET() {

  }

  void handlePOST() {

  }

private: 
  static asio::io_context ctx_;

  // client needs a socket, asio::io_context, ssl::context
  static unsigned responseCount_;
  static unsigned currentId_;

  std::mutex requestMutex_;

  std::map<unsigned, HttpsResponse*> cachedResponses_;
  std::map<unsigned, std::shared_ptr<Session>> sessionMap_;
}