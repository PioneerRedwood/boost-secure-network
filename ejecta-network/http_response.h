#include "http_request.h"

class HttpResponse {
public:
  HttpResponse(HttpRequest* request);
  virtual ~HttpResponse();

  HttpRequest* getHttpRequest();
  
  bool isSucceed;

  std::vector<char>* getResponseData();

  int getResponseCode();

  const char* getErrorBuffer();

  void setSucceed(bool value);

  void setResponseData(std::vector<char>* data);

  void setResponseCode(int value);

  void setErrorBuffer(const char* value);

  void setEffectiveUrl(const char* url);
  std::string getEffectiveUrl();

  void setContentType(const char* contentType);
  std::string getContentType();

  void setCached(bool isCached);
  bool isCached();

  void setUtf16(bool isUtf16);
  bool isUtf16();

private:
  static int responseCount = 0;

private:
  HttpRequest* request;
  bool succeed;
  std::vector<char> responseData;
  int responseCode;
  std::string errorBuffer;
  std::string effectiveUrl;
  bool cached;
  bool isUtf16;
  std::string contentType;
}