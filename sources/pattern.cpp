// Copyright 2020 Your Name <your_email>

#include <pattern.hpp>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

bool correct_url(const std::string& url){
  auto temp = std::string("://");
  return std::search(url.begin(), url.end(), temp.begin(), temp.end()) != url.end();
}

std::pair<std::string, std::string> divide_url(const std::string& url){    // return host, target
  auto temp = std::string("://");
  auto pos = std::search(url.begin(), url.end(), temp.begin(), temp.end());
  auto uri = std::string(pos+3, url.end());
  auto pos2 = std::find(uri.begin(), uri.end(), '/');
  auto host = std::string(uri.begin(), pos2);
  auto target = std::string(pos2, uri.end());
  if (target.empty()){
    target = "/";
  }
  return {host, target};
}

std::string download_page(const std::string& url){
  try {
    auto p = divide_url(url);
    std::string host = p.first;
    std::string target = p.second;
    auto port = "80";
    int version = 11;

    net::io_context ioc;

    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(host, port);


    stream.connect(results);

    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;

    http::response<http::string_body> res;

    http::read(stream, buffer, res);

    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != beast::errc::not_connected) {
      throw beast::system_error{ec};
    }

    return res.body();
  } catch(const std::exception& ex){
    std::cerr << ex.what() << '\n';
    return "";
  }
}