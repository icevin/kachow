#include "session.hh"

#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

tcp::socket& session::socket()
  {
    return socket_;
  }

int session::start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    return 1;
  }

int session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::stringstream ss;
      std::string sub(data_);
      sub = sub.substr(0, bytes_transferred);
      ss.str("");

      ss << "HTTP/1.1 200 OK\r\n";
      ss << "Content-Type: text/plain\r\n";
      ss << "Content-Length: " << std::to_string(bytes_transferred) << "\r\n";
      ss << "\r\n";
      ss << sub;

      std::string re = ss.str(); 

      boost::asio::async_write(socket_,
          boost::asio::buffer(re, re.length()),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
      return 1;
    }
    else
    {
      delete this;
      return -1;
    }
  }

int session::handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      return 1;
    }
    else
    {
      delete this;
      return -1;
    }
  }
  
int session::test_start() {
  return start();
}

int session::test_handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
        return handle_read(error, bytes_transferred);
      }

int session::test_handle_write(const boost::system::error_code& error) {
  return handle_write(error);
}