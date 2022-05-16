#include "https_core.h"
#include "https_session.h"
// #include "https_client.h"

enum selection_code
{
  none,
  enter_command,
  show_last_request,
  show_last_response,
  quit,
};

void mainLoop() {
// main thread for processing input
  char *input = new char[128];

  // std::unique_ptr<https_client> client_ = std::make_unique<https_client>();

  bool loop_switch = true;
  while (loop_switch)
  {
    std::cin.clear(std::ios::goodbit);

    std::cout << "\nEnter the menu options\n";
    std::cout << "#1 enter command\t#2 show last request list\t#3 show last response list\t#q quit\n\n";

    int selection = selection_code::none;
    std::cin >> selection;
    std::cin.clear(std::ios::goodbit);

    switch (selection)
    {
    case selection_code::enter_command:
    {
      std::cout << "Enter command to send\n> ";
      std::cin.getline(input, 128);

      std::string command{input};
      // parsing the command
      std::cout << command << "\n";
      break;
    }
    case selection_code::show_last_request:
    {
      std::cout << "Show last request\n";
      break;
    }
    case selection_code::show_last_response:
    {
      std::cout << "Show last response\n";
      break;
    }
    case selection_code::none:
      std::cout << "Invalid options..\n";
    case selection_code::quit:
      std::cout << "\nquit\n";
    default:
    {
      loop_switch = false;
      break;
    }
    }
    std::memset(input, 0, 128);
  }
  delete[] input;
}

class single {
public:
  single(single& s) = delete; 
  void operator=(single const&) = delete;
  static single& getInstance() {
    static single instance;
    return instance;
  }

  void method() {
    std::cout << "single class method() ..\n";
  }

private:
  single() {}
};

int main(void)
{
  // HttpsClient::getInstance().addRequest("GET", "github.com", "/PioneerRedwood");
  // single::getInstance().method();

  asio::io_context ctx;
  std::shared_ptr<Session> session = std::make_shared<Session>(ctx);
  session.get()->send("GET", "github.com", "/PioneerRedwood");

  ctx.run();
  return 0;
}