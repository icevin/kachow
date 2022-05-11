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

class RequestHandlerNotFound : public RequestHandler {
 public:
  RequestHandlerNotFound() {};

  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res);
};

class RequestHandlerAPI : public RequestHandler {
 public:
  RequestHandlerAPI(std::string data_path, int prefix_length)
    : base_path(data_path), prefix_length_{prefix_length} {};

  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res);

 private:
  // parse url target, returns request id (or 0 if none)
  bool parse_url_target(const http::request<http::string_body> req, 
    boost::filesystem::path& full_path, int& id);
    
  boost::filesystem::path base_path;
  int prefix_length_;
};
