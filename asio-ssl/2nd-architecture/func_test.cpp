#include "uri.h"
#include <iostream>
#include <string>
#include <functional>

using functor = std::function<void(const std::string&)>;

class SimpleA {
public:
  void func1(const std::string& str) {
    std::cout << str << "\n";
  }

  void bindCallback(functor& callback) {
    callback = std::bind(&SimpleA::func1, this, std::placeholders::_1);
  }
};

int main(void) {
  // SimpleA sa;
  // functor func;
  // sa.bindCallback(func);
  // func("Hello world!");
  
  Uri uri = Uri::parse(std::string{"https://www.notion.so/d73566e388ea4954ad2455511fe569f9?v=fe4ccf920bda452486e68da1d2eb9b81"});
  std::cout << "URI "
            << "[ protocol: " << uri.protocol 
            << " host: " << uri.host 
            << " port: " << uri.port
            << " path: " << uri.path
            << " query: " << uri.query << " ]\n";
}