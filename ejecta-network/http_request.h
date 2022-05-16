#include <vector>
#include <curl/curl.h>

class HttpClient;
class HttpResponse;

using void (*selectResponse)(HttpClient* client, HttpResponse* response);

class HttpRequest {
public:
  enum RequestType {
    GET,
    POST,
    PUT,
    DELETE,
    UNKNOWN
  };

  HttpRequest() {
    requestType_ = RequestType::UNKNOWN;
    url_.clear();
    requestData_.clear();
    tag_.clear();
    target_ = NULL;
    
    userData_ = NULL;
    timeout_ = 0;
    responseDataNeeded_ = true;
    cacheable_ = false;
  };

  virtual ~HttpRequest() {};

  void setRequestType(RequestType type) {
    requestType_ = type;
  };
  RequestType getRequestType() {
    return requestType_;
  };

  void setUrl(const char* url) {
    url_ = url;
  }
  const char* getUrl() {return url_.c_str();};

  void setRequestData(const char* buffer, size_t len) {
    requestData_.assign(buffer, buffer + len);
  };
  const char* getRequestData() {
    return &(requestData_.front());
  };
  int requestDataSize() {
    requestData_.size();
  };

  void setTag(const char* tag) {
    tag_ = tag;
  };
  const char* getTag() {
    return tag_.c_str();
  };

  void setUserData(void* userData) {
    userData_ = userData;
  };
  void* getUserData() {
    return userData_;
  };

  void setResponseCallback() {
    target_ = target;
    selector_ = selector;
  };

  void* getTarget() {
    return target_;
  };
  
  int getTimeout() {
    return timeout;
  };
  void setTimeout(int timeout) {
    timeout_ = timeout;
  };

  void setHeaders(std::vector<std::string> headers) {
    headers_ = headers;
  };
  std::vector<std::string> getHeaders() {
    return headers_;
  };

  bool isCacheable() {
    return cacheable_;
  };
  bool setCacheable(bool cacheable) {
    cacheable_ = cacheable;
  };

  void setResponseDataNeeded(bool needed) {
    responseDataNeeded_ = needed;
  };
  bool getResponseDataNeeded() {
    return responseDataNeeded_;
  };

  void startTiming() {
    clock_gettime(CLOCK_MONOTONIC, &start);
  };
  float finishTiming();

  struct timespec start;
  unsigned long total;

private:
  RequestType requestType_;
  std::string url_;
  std::vector<char> requestData_;
  std::string tag_;
  
  void* userData_;
  std::vector<std::string> headers_;
  int timeout_;
  bool responseDataNeeded_;
  bool cacheable_;

  ResponseSelector selector_;
}