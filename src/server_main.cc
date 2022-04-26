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
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

using boost::asio::ip::tcp;

void init()
{
    // enable logging to console
    logging::add_console_log(std::cout, keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%]: %Message%");
    
    // enable logging to file
    logging::add_file_log
    (
        keywords::file_name = "../log/server_%N.log",
        // rotate when log reaches 10 MB
        keywords::rotation_size = 10 * 1024 * 1024,
        // rotate daily at 00:00 UTC
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%]: %Message%"
    );
}

// handler to log shut down before program exit
void signal_handler (int signal_num) {
  BOOST_LOG_TRIVIAL(info) << "Termination signal received";
  BOOST_LOG_TRIVIAL(info) << "Shutting down server";
  exit(signal_num);
}

int main(int argc, char* argv[])
{
  init();
  logging::add_common_attributes();

  // register signal handler
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
    //// Get info on request handlers
    std::vector<std::vector<std::string>> handler_statements = config.getRequestHandlerStatements();

    BOOST_LOG_TRIVIAL(info) << "Attempting to start server on port " << portNumber;
    server s(io_service, portNumber, handler_statements);
    BOOST_LOG_TRIVIAL(info) << "Successfully started server on port " << portNumber;

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
