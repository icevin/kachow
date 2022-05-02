#pragma once

#include "request_handler.hh"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

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
  bool url_prefix_matches(const std::string request_str, const std::string url_prefix);

 private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int handle_write(const boost::system::error_code& error);

  tcp::socket socket_;
  enum { max_length = 1024, header_length = 63 };
  char data_[max_length];
  std::string resp;
  std::vector<std::vector<std::string>> handler_statements_;
};
