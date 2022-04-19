#include <cstdlib>
#include <iostream>
#include <functional>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

enum receive_type
{
  status_code,
  header,
  body
};