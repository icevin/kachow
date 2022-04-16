#include "server.hh"
#include "session.hh"

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, int port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

int server::start_accept()
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
    return 1;
  }

int server::handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    int ret = 0;
    if (!error)
    {
      new_session->start();
      ret = 1;
    }
    else
    {
      delete new_session;
      ret = -1;
    }

    start_accept();
    return ret;
  }
  
int server::test_start_accept() {
  return start_accept();
}

int server::test_handle_accept(session* new_session,
      const boost::system::error_code& error) {
    return handle_accept(new_session, error);
  }
