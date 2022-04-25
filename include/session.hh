#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service);

  tcp::socket& socket();

  int start();

  int test_start();
  int test_handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  int test_handle_write(const boost::system::error_code& error);

private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  int handle_write(const boost::system::error_code& error);

  tcp::socket socket_;
  enum { max_length = 1024, header_length = 63 };
  char data_[max_length];
  std::string resp;
};
