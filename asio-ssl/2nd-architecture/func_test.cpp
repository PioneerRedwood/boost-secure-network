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
  SimpleA sa;
  functor func;
  sa.bindCallback(func);
  func("Hello world!");
}