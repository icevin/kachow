#include "request_handler.hh"

#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

#include "mime.hh"

namespace http = boost::beast::http;
namespace fs = boost::filesystem;

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

bool RequestHandlerNotFound::get_response(const http::request<http::string_body> req,
                                     http::response<http::string_body>& res) {
    BOOST_LOG_TRIVIAL(info) << "404 Not Found: Prefix not matched";

    // Fill in 404 response
    res.version(11);
    res.result(http::status::not_found);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>404 Not Found</h1></html>";
    // Fill in content length
    res.prepare_payload();

    BOOST_LOG_TRIVIAL(info) << "Created Response: 404 Not Found";
    return true;
}


bool RequestHandlerAPI::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Serving API Request";

  // check request method
  auto method_sv = req.method_string();
  std::string method(method_sv);
  // declare variables
  int id = 0;
  fs::path full_path;
  std::string content_type;
  std::string response_body = "";
  // parse url path
  parse_url_target(req, full_path, id);
  // Create
  if (method == "POST") {
    // returns 400 full_path not 1 level lower than root
    if (full_path.parent_path() != base_path) {
      res.result(http::status::bad_request);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>400 Bad Request</h1></html>";
      res.prepare_payload();
      BOOST_LOG_TRIVIAL(error) << "API posting: bad target: " << full_path.string() << std::endl;
      return false;
    }
    // create entity if not already exists
    if (!fs::exists(full_path)) {
      fs::create_directory(full_path);
    }
    // TODO: get id
    id = 0;
    full_path /= std::to_string(id);
    // create entity file
    fs::ofstream entity(full_path);
    entity << req.body();
    entity.close();
    // construct 201 Created message
    res.result(http::status::created);
    res.body() = "{\"id\": " + std::to_string(id) + "}";
    res.prepare_payload();
    BOOST_LOG_TRIVIAL(info) << "API posting " << full_path.string() << std::endl;
  }
  // Retrieve or List
  else if (method == "GET") {
    // List
    if (id == 0) {
      // path not found, return 494
      if (!fs::exists(full_path)) {
        BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/html");
        res.body() = "<html><h1>404 Not Found</h1></html>";
        res.prepare_payload();
        return false;
      } 
      // iterate through all regular files
      for (const auto & entry : fs::directory_iterator(full_path)){
        if (fs::is_regular_file(entry.status())) {
          // TODO: implement List 
        }
      }
      // set content type
      content_type = "application/json";
      // convert json array to string
      BOOST_LOG_TRIVIAL(info) << "API listing " << full_path.string() << std::endl;
    }
    // Retrive 
    else {
      // file not found, return 404
      if (!fs::exists(full_path) || !fs::is_regular_file(full_path)) {
        BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/html");
        res.body() = "<html><h1>404 Not Found</h1></html>";
        res.prepare_payload();
        return false;
      } 
      // file found, get file
      else {
        // get content type
        content_type = extensionToMIME(full_path.extension().string());
        // get file contents
        std::ostringstream buf;
        std::ifstream input(full_path.c_str());
        buf << input.rdbuf();
        response_body  = buf.str();

        BOOST_LOG_TRIVIAL(info) << "API Retrieving " << full_path.string();
      }
    }
    // fill in response
    res.result(http::status::ok);
    res.set(http::field::content_type, content_type);
    res.body() = response_body;
    res.prepare_payload();
  }
  // Update 
  else if (method == "PUT") {
    // return 404 if path is not an existing regular file
    if (!fs::exists(full_path) || !fs::is_regular_file(full_path)) {
      BOOST_LOG_TRIVIAL(error) << "API PUT: not found for " << full_path.string();
      res.result(http::status::not_found);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>404 Not Found</h1></html>";
      res.prepare_payload();
    }
    // overwrite entity file
    fs::ofstream entity(full_path);
    entity << req.body();
    entity.close();
    // construct 200 ok message
    res.result(http::status::ok);
    res.prepare_payload();
    BOOST_LOG_TRIVIAL(info) << "API putting " << full_path.string() << std::endl;
  }
  // Delete
  else if (method == "DELETE") {
    // file not found or not regular file, returns 404 (not found)
    if (!fs::exists(full_path) || !fs::is_regular_file(full_path)) {
      BOOST_LOG_TRIVIAL(error) << "API DELETE: not found for " << full_path.string();
      res.result(http::status::not_found);
      res.set(http::field::content_type, "text/html");
      res.body() = "<html><h1>404 Not Found</h1></html>";
      res.prepare_payload();
    } 
    fs::remove(full_path);
    res.result(http::status::ok);
    BOOST_LOG_TRIVIAL(error) << "API DELETE: removed " << full_path.string();
    res.prepare_payload();
  }
  // Unsupported method
  else if (method == "HEAD" || method == "CONNECT" || method == "OPTIONS" ||
      method == "TRACE" || method == "PATCH") {
    BOOST_LOG_TRIVIAL(error) << "Received Unsupported Request";
    res.result(http::status::method_not_allowed);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>405 Not Allowed</h1></html>";
    res.prepare_payload();
    return false;
  } 
  // Bad Request
  else {
    BOOST_LOG_TRIVIAL(error) << "Received Bad Request";
    res.result(http::status::bad_request);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>400 Bad Request</h1></html>";
    res.prepare_payload();
    return false;
  }

  BOOST_LOG_TRIVIAL(info) << "Created Response: " << std::to_string(res.result_int());
  return true;
}

bool RequestHandlerAPI::parse_url_target(const http::request<http::string_body> req, 
    boost::filesystem::path& full_path, int& id) {
  // construct full file path
  auto url_sv = req.target();
  std::string url(url_sv);
  // this is here to deprecate prefix_length_ in future change
  int prefix_length = prefix_length_;
  url = url.substr(prefix_length);
  full_path = base_path;
  full_path /= url;
  // get the request id (if any)
  size_t pos = url.find_last_of('/');
  if (pos == std::string::npos) { // "/" not existed in path
    id = 0;
  }
  else {
    try {
      id = std::stoi(url.substr(pos + 1)); // convert id to int
    }
    catch(std::exception &err) { // path not ended with number
      id = 0;
    }
  }
  // parse success
  return true;
}