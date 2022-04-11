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

void session::start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      // get content-length
      std::string s = std::to_string(bytes_transferred);
      const char * cl = s.c_str();
      size_t num_digits = s.length();

      // add header field to echo response
      char response[max_length + header_length + num_digits];
      strcpy(response, "HTTP/1.1 200 OK\r\n");
      strcat(response, "Content-Type: text/plain\r\n");
      strcat(response, "Content-Length: ");
      strcat(response, cl);
      strcat(response, "\r\n\r\n");
      strcat(response, data_);

      boost::asio::async_write(socket_,
          boost::asio::buffer(response, bytes_transferred + header_length + num_digits),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

void session::handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }
  