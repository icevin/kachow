#pragma once

#include "filesystem.hh"
#include "token_auth.hh"

#include <map>
#include <set>
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>

namespace http = boost::beast::http;

class RequestHandler {
  public:
    virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) = 0;
    virtual std::string get_name() = 0;
};

class RequestHandlerEcho: public RequestHandler {
  public:
    RequestHandlerEcho() {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "Echo";};
};

class RequestHandlerHealth: public RequestHandler {
  public:
    RequestHandlerHealth() {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "Health";};
};

class RequestHandlerStatic: public RequestHandler {
  public:
    RequestHandlerStatic(std::string serve_path, int prefix_length) 
      : base_path(serve_path), prefix_length_{prefix_length} {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "Static";};
  
  private:
    boost::filesystem::path base_path;
    int prefix_length_;
};

class RequestHandlerNotFound : public RequestHandler {
  public:
    RequestHandlerNotFound() {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "NotFound";};
};

class RequestHandlerAPI : public RequestHandler {
  public:
    RequestHandlerAPI(std::string data_path, int prefix_length, std::map<std::string, std::set<int>>* entity_id_map, FileSystem* fs) 
    : base_path(data_path), entity_id_map_(entity_id_map), prefix_length_(prefix_length), file(fs) {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "API";};

  private:
    // parse url target, returns request id (or 0 if none)
    bool parse_url_target(const http::request<http::string_body> req, boost::filesystem::path& full_path, int& id, std::string& entity);
    // given entity, return next available ID
    int get_next_id(std::string entity);
    // helper function, set res to 400 bad request
    void set_bad_request(http::response<http::string_body>& res);
    // helper function, set res to 404 not found
    void set_notfound_request(http::response<http::string_body>& res);
    // erase the entity with id from the map if it exist
    void erase_if_exist(std::string entity, int id);
    // insert the entity with id to the map if it does not exist
    void insert_if_notexist(std::string entity, int id);
    boost::filesystem::path base_path;
    int prefix_length_;
    // map with key = entity name, value = set of IDs
    std::map<std::string, std::set<int>>* entity_id_map_;
    FileSystem* file;
};

class SecureRequestHandlerAPI : public RequestHandler {
  public:
    SecureRequestHandlerAPI(std::string data_path, int prefix_length, std::map<std::string, std::set<int>>* entity_id_map, FileSystem* fs, Auth* authorizer);
    bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "SecureAPI";};

  private:
    RequestHandler* base_request_handler_priv;
    RequestHandler* base_request_handler_pub;
    Auth* authorizer_;
    int prefix_length_;
};

class RequestHandlerSleep : public RequestHandler {
 public:
  RequestHandlerSleep() {};

  virtual bool get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res);
  std::string get_name() {return "Sleep";};
};

class RequestHandlerLogin: public RequestHandler {
  public:
    RequestHandlerLogin(std::string serve_path) 
      : base_path(serve_path) {};

    virtual bool get_response(const http::request<http::string_body> req, 
      http::response<http::string_body>& res);
    std::string get_name() {return "Login";};
  
  private:
    boost::filesystem::path base_path;
    int prefix_length_;
    std::string login_path = "/login.html";
};
