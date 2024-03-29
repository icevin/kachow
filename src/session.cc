#include "session.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

using boost::asio::ip::tcp;

std::map<std::string, RequestHandlerFactory*> session::routes_;

session::session(boost::asio::io_service& io_service, boost::asio::ssl::context& context,
  std::map<std::string, RequestHandlerFactory*> routes)
    : socket_(io_service, context) {
  routes_ = routes;
}

ssl_socket::lowest_layer_type& session::socket() {
  return socket_.lowest_layer();
}

int session::start() {
  socket_.async_handshake(boost::asio::ssl::stream_base::server,
    boost::bind(&session::handle_handshake, this,
      boost::asio::placeholders::error));
  return 1;
}

int session::handle_handshake(const boost::system::error_code& error) {
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

int session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
  if (!error) {
      auto method_sv = request_.method_string();
      auto target_sv = request_.target();
      std::string method(method_sv);
      std::string target(target_sv);

    std::string location = session::match(routes_, target);
    BOOST_LOG_TRIVIAL(debug) << "got location from session::match: " << location << "\n";

    // Checks if a matching handler exists
    std::string handler_name = "NA";
    if (routes_.count(location) == 0) {
      BOOST_LOG_TRIVIAL(error) << "Received Invalid Request";

      response_.version(11);
      response_.result(http::status::bad_request);
      response_.set(http::field::content_type, "text/html");
      response_.body() = "<html><h1>400 Bad Request</h1></html>";
      response_.prepare_payload();

      BOOST_LOG_TRIVIAL(info) << "Created Response: 400 Bad Request";

      // 404 Version (unused)
      // RequestHandlerFactory* factory = new NotFoundHandlerFactory();
      // RequestHandler* handler = factory->create(location, target);
      // BOOST_LOG_TRIVIAL(debug) << "created handler using factory\n";
      // handler->get_response(request_, response_);
      // delete handler;
    } else {
      RequestHandlerFactory* factory = routes_[location];
      BOOST_LOG_TRIVIAL(debug) << "got factory pointer from routes_\n";
      RequestHandler* handler = factory->create(location, target);
      BOOST_LOG_TRIVIAL(debug) << "created handler using factory\n";
      handler->get_response(request_, response_);
      handler_name = handler->get_name();
      delete handler;
    }
    BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics] {response_code: " << response_.result_int() << ", "
                                                  "request_path: " << target << ", "
                                                  "request_handler_name: " << handler_name << ", "
                                                  "request_ip: " << this->socket().remote_endpoint().address().to_string() << ",}\n";

    http::async_write(socket_, response_,
        boost::bind(&session::handle_write, this,
          boost::asio::placeholders::error));
    return 1;
  } else {
      if (bytes_transferred > 0) {
        BOOST_LOG_TRIVIAL(error) << "Received Invalid Request";

        response_.version(11);
        response_.result(http::status::bad_request);
        response_.set(http::field::content_type, "text/html");
        response_.body() = "<html><h1>400 Bad Request</h1></html>";
        response_.prepare_payload();

        BOOST_LOG_TRIVIAL(info) << "Created Response: 400 Bad Request";

        http::async_write(socket_, response_,
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
      }

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

// Matches a request url (e.g. /static/kachow.jpeg) to a location (e.g. /static/)
// - uses longest prefix matching to make the choice
// -> returns the location as a string
std::string session::match(std::map<std::string, RequestHandlerFactory*> routes, std::string request_url) {
  std::map<std::string, RequestHandlerFactory*>::iterator itr;
  int longest_match = 0;
  std::string longest_match_location;
  for (itr = routes.begin(); itr != routes.end(); ++itr) {
    std::string sub_url = request_url.substr(0, (itr->first).length());
    if (sub_url == itr->first && sub_url.length() > longest_match) {
      longest_match = sub_url.length();
      longest_match_location = sub_url;
    }
  }
  return longest_match_location;
}
