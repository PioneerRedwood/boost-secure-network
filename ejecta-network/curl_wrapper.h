using size_t (*write_callback)(void* ptr, size_t size, size_t nmemb, void* stream);
#include <curl/curl.h>
// #include <curl/options.h>

class CurlWrapper {
public:
  static size_t readHeader(char* buffer, size_t size, size_t nItems, void* userData);

public:
  CurlWrapper();
  ~CurlWrapper();

  void cleanCurl();
  CURL* getCurl();
  char* getErrorBuffer();

  template<class T>
  bool setOptions(CURLOption option, T data);
  bool init(HttpRequest* request, HttpResponse* response, write_callback callback, 
    void* stream, int connectTimeout, int readTimeout, const char* proxyString, const char* hostString = NULL);
  
  bool configureForGet();
  bool configureForPost(HttpRequest* request);
  bool configureForPut(HttpRequest* request);
  bool configureForDelete();

  char* parseValue(char* buffer, char* delimiter, size_t pos);
  char* parseValue(char* buffer, char* delimiter);
  size_t readHeader(char* buffer, size_t size, size_t nItems);

private:
  static CurlWrapper* curlWrapper;
  CURL* curl;
  curl_slist* headers;
  char* currentUrl;
  char errorBuffer[CURL_ERROR_SIZE];
  char* curlCookies;
};

