#pragma once

#include "session.hh"
#include "config_parser.hh"
#include "request_handler_factory.hh"
#include "filesystem.hh"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class server {
 public:
  server(boost::asio::io_service& io_service, int port,
    	const char* pk_file, const char* pem_file, int num_threads = 1);
  int test_start_accept();
  int test_handle_accept(session* new_session,
      const boost::system::error_code& error);
  static std::map<std::string, RequestHandlerFactory*> get_routes();
  static void generate_routes(NginxConfig* config, FileSystem* fs);
  void run();

 private:
  int number_threads = 1;
  int start_accept();
  int handle_accept(session* new_session,
      const boost::system::error_code& error);
      
  static std::map<std::string, RequestHandlerFactory*> routes_;
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  boost::asio::ssl::context context_;
};
