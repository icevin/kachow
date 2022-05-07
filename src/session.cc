#include "session.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service, std::vector<std::vector<std::string>> handler_statements)
    : socket_(io_service) {
    handler_statements_ = handler_statements;
  }

tcp::socket& session::socket() {
    return socket_;
  }

int session::start() {
  http::async_read(socket_, buffer_, request_,
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    return 1;
  }

int session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
    if (!error) {
        auto method_sv = request_.method_string();
        auto target_sv = request_.target();
        std::string method(method_sv);
        std::string target(target_sv);

        BOOST_LOG_TRIVIAL(info) << method << " " << target << " " << "HTTP/1.1" 
          << " (" << socket_.remote_endpoint().address().to_string() << ")";

      // Choose the appropriate RequestHandler based on the handler_statements_ object
      RequestHandler* handler = nullptr;
      for (const std::vector<std::string> statement : handler_statements_) {
        // check for matching url prefix
        std::string handler_type = statement[0];
        std::string prefix = statement[1];
        if (session::url_prefix_matches(target, prefix)) {
          if (handler_type == "echo") {
            handler = new RequestHandlerEcho();
            // BOOST_LOG_TRIVIAL(debug) << "RequestHandlerEcho chosen\n";
          } else if (handler_type == "static_serve") {
            // MODIFY HERE LATER TO ADD SUPPORT FOR RequestHandlerStaticServe
            handler = new RequestHandlerStatic(statement[2], prefix.length());
            // BOOST_LOG_TRIVIAL(debug) << "RequestHandlerStatic chosen\n";
          }
        }
      }

      if (handler == nullptr) {
        BOOST_LOG_TRIVIAL(error) << "Received Invalid Request";

        response_.version(11);
        response_.result(http::status::bad_request);
        response_.set(http::field::content_type, "text/html");
        response_.body() = "<html><h1>400 Bad Request</h1></html>";
        response_.prepare_payload();

        BOOST_LOG_TRIVIAL(info) << "Created Response: 400 Bad Request";
      } else {
        handler->get_response(request_, response_);
        delete handler;
      }
      // BOOST_LOG_TRIVIAL(debug) << "Response generated: [" << this->resp << "]\n\n";

      http::async_write(socket_, response_,
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
      http::async_read(socket_, buffer_, request_,
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

bool session::url_prefix_matches(const std::string target, const std::string url_prefix) {
    std::string sub_url = target.substr(0, url_prefix.length());
    // std::cout << "sub_url: " << sub_url << "\n";
    return (sub_url == url_prefix);
}
