#pragma once

#include "request_handler.hh"
#include <string>

class RequestHandlerFactory {
 public:
  virtual RequestHandler* create(std::string location, std::string request_url) = 0;
};

class StaticHandlerFactory : public  RequestHandlerFactory{
 public:
  StaticHandlerFactory(std::string root_file_path) : root_file_path_(root_file_path) {};
  RequestHandler* create(std::string location, std::string request_url);
 private:
  std::string root_file_path_;
};

class EchoHandlerFactory : public  RequestHandlerFactory{
 public:
  EchoHandlerFactory() {};
  RequestHandler* create(std::string location, std::string request_url);
};

class NotFoundHandlerFactory : public RequestHandlerFactory{
  public:
    NotFoundHandlerFactory() {};
    RequestHandler* create(std::string location, std::string request_url);
};