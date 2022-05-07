#pragma once

#include "request_handler.hh"

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
  session(boost::asio::io_service& io_service, std::vector<std::vector<std::string>> handler_statements);
  tcp::socket& socket();
  int start();

  int test_start();
  int test_handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int test_handle_write(const boost::system::error_code& error);

  // PURE FUNCTION (move to utils)
  bool url_prefix_matches(const std::string target, const std::string url_prefix);

 private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int handle_write(const boost::system::error_code& error);

  tcp::socket socket_;
  boost::beast::flat_buffer buffer_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
  std::vector<std::vector<std::string>> handler_statements_;
};
