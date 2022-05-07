#include "mime.hh"
#include "request_handler.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

// handles request req by filling in response res with contents of
// req as a string, returns true to indicate success 
bool RequestHandlerEcho::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Received Echo Request";

  // convert request to string
  std::stringstream ss;
  ss << req;
  std:: string req_str = ss.str();

  // fill in echo response
  res.version(11);
  res.result(http::status::ok);
  res.set(http::field::content_type, "text/plain");
  res.body() = req_str;

  // fill in content length
  res.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Created Response: 200 OK";
  return true;
}

// handles request req by filling in response res with contents of
// target file or 400/404 error message, returns true to indicate success 
bool RequestHandlerStatic::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Received Static Request";

  // convert request to string
  std::stringstream ss;
  ss << req;
  std:: string request_str = ss.str();

  // find index for parsing
  std::size_t GET_INDEX  = request_str.find("GET ");
  std::size_t HTTP_INDEX = request_str.find(" HTTP/");
  std::string request_path;
    
  if (GET_INDEX == std::string::npos || HTTP_INDEX == std::string::npos) {
    // unable to find index
    BOOST_LOG_TRIVIAL(error) << "Received Malformed Request";
    res.version(11);
    res.result(http::status::bad_request);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>400 Bad Request</h1></html>";
  } else {
    // construct file path
    request_path = request_str.substr(GET_INDEX + 4 + prefix_length_, HTTP_INDEX - GET_INDEX - 4 - prefix_length_);
    BOOST_LOG_TRIVIAL(info) << "Received Static Request: " << request_path;
    boost::filesystem::path full_path = base_path;
    full_path /= request_path;
    if (!boost::filesystem::exists(full_path) || !boost::filesystem::is_regular_file(full_path)) {
      // file not found
      BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
      res.version(11);
      res.result(http::status::not_found);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>404 Not Found</h1></html>";
    } else {
      // file found
      std::string content_type = extensionToMIME(full_path.extension().string());
      std::ostringstream buf;
      std::ifstream input(full_path.c_str());
      buf << input.rdbuf();
      std::string response_body  = buf.str();
      res.version(11);
      res.result(http::status::ok);
      res.set(http::field::content_type, content_type);
      res.body() = response_body;
      BOOST_LOG_TRIVIAL(info) << "Serving " << full_path.string();
    }
  }

  // fill in content length
  res.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Created Response: ";
  return true;
}
