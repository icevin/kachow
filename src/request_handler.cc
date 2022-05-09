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
  BOOST_LOG_TRIVIAL(info) << "Serving Echo Request";

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
// target file or error message, returns true to indicate 200 response
// returns false to indicate 400, 404 or 405 response
bool RequestHandlerStatic::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Serving Static Request";

  // check request method
  auto method_sv = req.method_string();
  std::string method(method_sv);
  if (method != "GET") {
    if (method == "HEAD" || method == "POST" || method == "PUT" ||
        method == "DELETE" || method == "CONNECT" || method == "OPTIONS" ||
        method == "TRACE" || method == "PATCH") {
      BOOST_LOG_TRIVIAL(error) << "Received Unsupported Request";
      res.result(http::status::method_not_allowed);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>405 Not Allowed</h1></html>";
      res.prepare_payload();
      return false;
    } else {
      BOOST_LOG_TRIVIAL(error) << "Received Bad Request";
      res.result(http::status::bad_request);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>400 Bad Request</h1></html>";
      res.prepare_payload();
      return false;
    }
  }

  // construct full file path
  auto url_sv = req.target();
  std::string url(url_sv);
  // this is here to deprecate prefix_length_ in future change
  int prefix_length = prefix_length_;
  boost::filesystem::path full_path = base_path;
  full_path /= url.substr(prefix_length);

  // get file
  if (!boost::filesystem::exists(full_path) || !boost::filesystem::is_regular_file(full_path)) {
    // file not found
    BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
    res.result(http::status::not_found);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>404 Not Found</h1></html>";
    res.prepare_payload();
    return false;
  } else {
    // get content type
    std::string content_type = extensionToMIME(full_path.extension().string());

    // get file contents
    std::ostringstream buf;
    std::ifstream input(full_path.c_str());
    buf << input.rdbuf();
    std::string response_body  = buf.str();

    // fill in response
    res.result(http::status::ok);
    res.set(http::field::content_type, content_type);
    res.body() = response_body;
    res.prepare_payload();
    BOOST_LOG_TRIVIAL(info) << "Serving " << full_path.string();
  }

  BOOST_LOG_TRIVIAL(info) << "Created Response: " << std::to_string(res.result_int());
  return true;
}
