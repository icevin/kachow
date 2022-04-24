#include <string>
#include <sstream>
#include "request_handler.hh"

std::string RequestHandlerEcho::get_response(std::string request_str) { 
  std::stringstream ss;
  //std::string sub(data_);
  //sub = sub.substr(0, bytes_transferred);
  ss.str("");

  ss << "HTTP/1.1 200 OK\r\n";
  ss << "Content-Type: text/plain\r\n";
  ss << "Content-Length: " << std::to_string(request_str.length()) << "\r\n";
  ss << "\r\n";
  ss << request_str;

  return ss.str();
}