#include <curl/curl.h>
#include <pthread>
#include <map>
#include <list>
#include <semaphore.h>

class HttpClient {
public: // static
  static HttpClient* getInstance();
  static void destroyInstance();
  static void setProxy(const char* proxyDomain);
  static void handleCachedResponse(int id, int result);
  static void queueCacheResponse(HttpResponse* response);
  
public:
  void send(HttpRequest* request);
  CacheHint getCachedItem(const char* url);
  
  void setConnectTimeout(int value);
  int getConnectTimeout();

  void setReadTimeout(int value);
  int getReadTimeout();

  void dispatchResponseCallback();
  void abortAllRequest();

private: // static
  static size_t writeData(void* data, size_t size, size_t nmemb, void* stream);
  static void* networkThread(void* arg);
  static void handleCacheResponse(HttpResponse* response);
  static void queueCacheResponseFunc(void* arg);

private:
  HttpClient();
  virtual ~HttpClient();
  bool init(void);
  bool lazyInit();

private: // static
  static const char* systemProxyString;
  static ThreadPool cachedResponseHandlingPool;
  

private:
  int connectTimeout;
  int readTimeout;
  volatile int needQuit;
  CURLM* curlMultiHandle;
  CURLInstance* curlInstance;
  std::map<CURL*, CurlWrapper> curlMap;
  std::list<CurlWrapper> curlList;
  std::map<int, HttpResponse> cachedResponseMap;
  pthread_mutex_t cacheRequestMutex;
  sem_t quttingSemaphore;
}