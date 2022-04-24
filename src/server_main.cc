//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "session.hh"
#include "server.hh"
#include "config_parser.hh"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;

void signal_handler (int signal_num) {
  BOOST_LOG_TRIVIAL(info) << "Termination signal received";
  BOOST_LOG_TRIVIAL(info) << "Shutting down server";
  exit(signal_num);
}

int main(int argc, char* argv[])
{
  signal(SIGINT, signal_handler);

  try
  {
    BOOST_LOG_TRIVIAL(info) << "Starting up server";

    if (argc != 2)
    {
      std::cerr << "Usage: ./server <my_config>\n";
      BOOST_LOG_TRIVIAL(fatal) << "Config file not found";
      return 1;
    }

    // Parse config file
    BOOST_LOG_TRIVIAL(info) << "Attempting to parse config file";
    NginxConfigParser config_parser;
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config)) {
      BOOST_LOG_TRIVIAL(fatal) << "Unable to parse config file";
      return 1;
    }
    BOOST_LOG_TRIVIAL(info) << "Successfully parsed config file";

    boost::asio::io_service io_service;

    using namespace std; // For atoi.

    // Get port number from config and check for an error
    int portNumber = config.portNumber();
    if (portNumber == -1) {
      std::cerr << "Port number not found in config file\n";
      BOOST_LOG_TRIVIAL(fatal) << "Port number not found in config file";
      return 1;
    }
    BOOST_LOG_TRIVIAL(info) << "Attempting to start server on port " << portNumber;
    server s(io_service, portNumber);
    BOOST_LOG_TRIVIAL(info) << "Successfully started server on port " << portNumber;

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
