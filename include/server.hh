#pragma once

#include "session.hh"
#include "config_parser.hh"
#include "request_handler_factory.hh"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class server {
 public:
  server(boost::asio::io_service& io_service, int port, std::vector<std::vector<std::string>> handler_statements);
  int test_start_accept();
  int test_handle_accept(session* new_session,
      const boost::system::error_code& error);
  static std::map<std::string, RequestHandlerFactory*> get_routes();
  static void generate_routes(NginxConfig* config);

 private:
  int start_accept();
  int handle_accept(session* new_session,
      const boost::system::error_code& error);
      
  static std::map<std::string, RequestHandlerFactory*> routes_;
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  std::vector<std::vector<std::string>> handler_statements_;
};
