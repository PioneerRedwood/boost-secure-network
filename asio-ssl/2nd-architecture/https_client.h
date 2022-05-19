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

  void send(HttpsRequest* request) {
    // queue request into list
  }

  // for overloading adding reqeust
  void addRequest(const std::string& url) {

  }

  // add request
  void addRequest(const std::string& method, const std::string& server, const std::string& path) {
    // add request to request queue
    // std::shared_ptr<HttpsRequest> request = std::make_shared<HttpsRequest>(currentId_++);
    // request.get()->initRequest(method, server, path);
    
    // how about just make the limitation for sessions on here
    // add the check logic for idle session for handling request
    // thousand of session will be created here, 
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

  void dispatchCallback(HttpsResponse* response) {

  }

  void networkThread() {
    ctx_.run();
    auto loop = [&](){
      
      std::lock_guard<std::mutex> guard(requestMutex_);
      while(!requestList_.empty()) {
        send(requestList_.pop_front());
      }
    };

    std::thread thr{loop};
    thr.join();
  }

private: 
  static asio::io_context ctx_;

  // client needs a socket, asio::io_context, ssl::context
  static unsigned responseCount_;
  static unsigned currentId_;

  std::mutex requestMutex_;

  std::list<HttpsResponse*> responseList_;
  std::list<HttpsRequest*> requestList_;

  // std::map<unsigned, HttpsResponse*> cachedResponses_;
  std::map<unsigned, std::shared_ptr<Session>> sessionMap_;
}