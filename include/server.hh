#pragma once

#include "session.hh"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service, int port);
  int test_start_accept();
  int test_handle_accept(session* new_session,
      const boost::system::error_code& error);

private:
  int start_accept();

  int handle_accept(session* new_session,
      const boost::system::error_code& error);

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};
