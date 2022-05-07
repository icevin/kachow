#pragma once

#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>

namespace http = boost::beast::http;

class RequestHandler {
 public:
  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) = 0;
};

class RequestHandlerEcho : public RequestHandler {
 public:
  RequestHandlerEcho() {};

  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res);
};

class RequestHandlerStatic : public RequestHandler {
 public:
  RequestHandlerStatic(std::string serve_path, int prefix_length) 
    : base_path(serve_path), prefix_length_{prefix_length} {};

  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res);
  
 private:
  boost::filesystem::path base_path;
  int prefix_length_;
};
