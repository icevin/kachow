#pragma once

#include "request_handler.hh"
#include "request_handler_factory.hh"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>

namespace http = boost::beast::http;

using boost::asio::ip::tcp;

class session {
 public:
  session(boost::asio::io_service& io_service, std::map<std::string, RequestHandlerFactory*> routes);
  tcp::socket& socket();
  int start();

  int test_start();
  int test_handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int test_handle_write(const boost::system::error_code& error);

  // PURE FUNCTION
  static std::string match(std::map<std::string, RequestHandlerFactory*> routes, std::string request_url);

 private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int handle_write(const boost::system::error_code& error);
  static std::map<std::string, RequestHandlerFactory*> routes_;

  tcp::socket socket_;
  boost::beast::flat_buffer buffer_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
};
