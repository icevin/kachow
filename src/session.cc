#include "session.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service, std::vector<std::vector<std::string>> handler_statements)
    : socket_(io_service) {
    handler_statements_ = handler_statements;
  }

tcp::socket& session::socket() {
    return socket_;
  }

int session::start() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    return 1;
  }

int session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
    if (!error) {
      std::string sub(data_);

      if (bytes_transferred > 0) {
        sub = sub.substr(0, bytes_transferred);
        std::size_t first_cr = sub.find('\r');
        std::size_t first_lf = sub.find('\n');
        std::size_t header_end = std::min(first_cr, first_lf);

        BOOST_LOG_TRIVIAL(info) << sub.substr(0, header_end) << " (" 
          << socket_.remote_endpoint().address().to_string() << ")";
      }
      
      // Choose the appropriate RequestHandler based on the handler_statements_ object
      std::string request_str = sub;
      RequestHandler* handler = nullptr;
      for (const std::vector<std::string> statement : handler_statements_) {
        // check for matching url prefix
        std::string handler_type = statement[0];
        std::string prefix = statement[1];
        std::string request = statement[2];
        if (session::url_prefix_matches(request_str, prefix)) {
          if (handler_type == "echo") {
            handler = new RequestHandlerEcho();
            // BOOST_LOG_TRIVIAL(debug) << "RequestHandlerEcho chosen\n";
          } else if (handler_type == "static_serve") {
            // MODIFY HERE LATER TO ADD SUPPORT FOR RequestHandlerStaticServe
            handler = new RequestHandlerStatic(request, prefix.length());
            // BOOST_LOG_TRIVIAL(debug) << "RequestHandlerStatic chosen\n";
          }
        }
      }
      if (handler == nullptr) {
        BOOST_LOG_TRIVIAL(error) << "Received Invalid Request";

        this->resp = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n"
          "Content-Length: 37\r\n\r\n<html><h1>400 Bad Request</h1></html>";

        BOOST_LOG_TRIVIAL(info) << "Created Response: 400 Bad Request";
      } else {
        this->resp = handler->get_response(sub);
        delete handler;
      }
      // BOOST_LOG_TRIVIAL(debug) << "Response generated: [" << this->resp << "]\n\n";

      boost::asio::async_write(socket_,
          boost::asio::buffer(this->resp, this->resp.length()),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
      return 1;
    } else {
      delete this;
      return -1;
    }
  }

int session::handle_write(const boost::system::error_code& error) {
    if (!error) {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      return 1;
    } else {
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

bool session::url_prefix_matches(const std::string request_str, const std::string url_prefix) {
    std::regex rgx("GET (.+) HTTP");

    std::smatch matches;
    std::regex_search(request_str, matches, rgx);
    std::string url_string = matches[1].str();

    std::string sub_url = url_string.substr(0, url_prefix.length());
    // std::cout << "sub_url: " << sub_url << "\n";
    return (sub_url == url_prefix);
}