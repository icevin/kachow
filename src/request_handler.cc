#include "request_handler.hh"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <sstream>
#include <string>
RequestHandlerEcho::RequestHandlerEcho(){
  
}

#include "mime.hh"

std::string RequestHandlerEcho::get_response(std::string request_str) {
    BOOST_LOG_TRIVIAL(info) << "Received Echo Request";

    std::stringstream ss;
    ss.str("");

    ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-Type: text/plain\r\n";
    ss << "Content-Length: " << std::to_string(request_str.length()) << "\r\n";
    ss << "\r\n";
    ss << request_str;

    BOOST_LOG_TRIVIAL(info) << "Created Response: 200 OK";
    return ss.str();
}

std::string RequestHandlerStatic::get_response(std::string request_str) {
    std::stringstream ss;
    ss.str("");

    std::string status_code    = "200 OK";
    std::string content_type   = "text/plain";
    std::string content_length = std::to_string(request_str.length());
    std::string response_body  = "";

    std::size_t GET_INDEX  = request_str.find("GET ");
    std::size_t HTTP_INDEX = request_str.find(" HTTP/");
    std::string request_path;

    if (GET_INDEX == std::string::npos || HTTP_INDEX == std::string::npos) {
        BOOST_LOG_TRIVIAL(error) << "Received Malformed Request";
        status_code   = "400 Bad Request";
        response_body = "<html><h1>400 Bad Request</h1></html>";
        content_length = std::to_string(response_body.size());
        content_type   = "text/html";
    } else {
        request_path = request_str.substr(GET_INDEX + 4 + prefix_length_, HTTP_INDEX - GET_INDEX - 4 - prefix_length_);
        BOOST_LOG_TRIVIAL(info) << "Received Static Request: " << request_path;
        boost::filesystem::path full_path = base_path;
        full_path /= request_path;
        if (!boost::filesystem::exists(full_path) || !boost::filesystem::is_regular_file(full_path)) {
            BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
            status_code    = "404 Not Found";
            response_body  = "<html><h1>404 Not Found</h1></html>";
            content_length = std::to_string(response_body.size());
            content_type   = "text/html";
        } else {
            content_type = extensionToMIME(full_path.extension().string());
            std::ostringstream buf;
            std::ifstream input(full_path.c_str());
            buf << input.rdbuf();
            response_body  = buf.str();
            content_length = std::to_string(response_body.size());
            BOOST_LOG_TRIVIAL(info) << "Serving " << full_path.string() << ", size " << content_length;
        }
    }

    // Assemble response header - at some point we should adapt to use boost::http:response
    ss << "HTTP/1.1 " << status_code << "\r\n";
    ss << "Content-Type: " << content_type << "\r\n";
    if (content_length != "") ss << "Content-Length: " << content_length << "\r\n";
    ss << "\r\n";

    // Send body if applicable
    if (response_body != "") ss << response_body;

    BOOST_LOG_TRIVIAL(info) << "Created Response: " << status_code;
    return ss.str();
}