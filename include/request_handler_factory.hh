#pragma once

#include "request_handler.hh"
#include <boost/log/trivial.hpp>
#include <string>
#include <map>
#include <set>
namespace fs = boost::filesystem;
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

class APIHandlerFactory : public RequestHandlerFactory{
 public:
  APIHandlerFactory(std::string root_file_path) : root_file_path_(root_file_path) {
  if(!scanFS())
  {
    BOOST_LOG_TRIVIAL(warning) << "CURD init root path not exist, created." << root_file_path;
  }
  };
  RequestHandler* create(std::string location, std::string request_url);
 private:
  // scan file system, and put entity and ID into the entity_id_map if exists
  // return false if root path does not exist
  bool scanFS();
  // given entry for a entity, scan IDs and insert into the map
  void Insert_ID(fs::directory_entry entry);
  std::string root_file_path_;
  // map with key = entity name, value = set of IDs
  std::map<std::string, std::set<int>> entity_id_map;
};