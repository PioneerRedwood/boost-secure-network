#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>

namespace asio = boost::asio;

void direct_insert(asio::streambuf& sb, std::string const& data)
{
    auto size = data.size();
    auto buffer = sb.prepare(size);
    std::copy(begin(data), end(data), asio::buffer_cast<char*>(buffer));
    sb.commit(size);
}

void stream_insert(asio::streambuf& sb, std::string const& data)
{
    std::ostream strm(std::addressof(sb));
    strm << data;
}

std::string extract_istream(asio::streambuf& sb)
{
    std::istream is(std::addressof(sb));
    std::string line;
    std::getline(is, line);
    return line;
}

std::string extract_direct(asio::streambuf& sb)
{
    auto buffer = sb.data();
    auto first = asio::buffer_cast<const char*>(buffer);
    auto bufsiz = asio::buffer_size(buffer);
    auto last = first + bufsiz;

    auto nlpos = std::find(first, last, '\n');

    auto result = std::string(first, nlpos);

    auto to_consume = std::min(std::size_t(std::distance(first, nlpos) + 1), bufsiz);
    sb.consume(to_consume);
    return result;
}

std::string extract_custom(asio::streambuf& sb) {
    auto buffer = sb.data();
    auto first = asio::buffer_cast<const char*>(buffer);
    auto size = asio::buffer_size(buffer);
    auto result = std::string(first, size);
    
    sb.consume(size);
    return result;
}

int main()
{
    asio::streambuf buf;
    // direct_insert(buf, "The cat sat on the mat\n");
    // stream_insert(buf, "The cat sat on the mat\n");
    stream_insert(buf, "The cat sat on the mat");

    // auto s1 = extract_direct(buf);
    // auto s2 = extract_istream(buf);
    auto s3 = extract_custom(buf);

    // std::cout << s1 << "\n" << s2 << "\n";
    std::cout << s3 << "\n";
}