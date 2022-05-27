#include "mime.hh"
#include "request_handler.hh"

#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>

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

// handles request req by filling in response res with OK
// returns true to indicate success 
bool RequestHandlerHealth::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Serving Health Request";

  // fill in health response
  res.version(11);
  res.result(http::status::ok);
  res.set(http::field::content_type, "text/plain");
  res.body() = "OK";

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

// helper function, set res to 404 not found
void RequestHandlerAPI::set_notfound_request(http::response<http::string_body>& res) {
  res.result(http::status::not_found);
  res.set(http::field::content_type, "text/html");
  res.body() = "<html><h1>404 Not Found</h1></html>";
  res.prepare_payload();
}

// helper function, set res to 400 bad request
void RequestHandlerAPI::set_bad_request(http::response<http::string_body>& res) {
  res.result(http::status::bad_request);
  res.set(http::field::content_type, "text/html");
  res.body() = "<html><h1>400 Bad Request</h1></html>";
  res.prepare_payload();
}

// given entity, return next available ID
int RequestHandlerAPI::get_next_id(std::string entity) {
  int id = 1;
  // when call this function, the map must have key of entity, no need to check
  std::map<std::string, std::set<int>>::iterator entityIter = entity_id_map_->find(entity); 
  // current IDs for the entity
  std::set<int> existingIDs = entityIter -> second; 
  // loop to find next available ID
  while(existingIDs.find(id) != existingIDs.end())
    id++;
  // update map
  existingIDs.insert(id);
  entityIter -> second = existingIDs;
  return id;
}

// erase the entity with id from the map if it exist
void RequestHandlerAPI::erase_if_exist(std::string entity, int id) {
  std::map<std::string, std::set<int>>::iterator entityIter = entity_id_map_->find(entity); 
  // if entity exist
  if(entityIter != entity_id_map_ -> end()) {
    std::set<int> existingIDs = entityIter -> second; 
    std::set<int>::iterator it = existingIDs.find(id);
    // if id exist
    if(it != existingIDs.end()) {
      // erase and update
      existingIDs.erase(id);
      entityIter -> second = existingIDs;
    }
  }
}

// insert the entity with id to the map if it does not exist
void RequestHandlerAPI::insert_if_notexist(std::string entity, int id) {
  std::map<std::string, std::set<int>>::iterator entityIter = entity_id_map_->find(entity); 
  // if entity exists
  if(entityIter != entity_id_map_ -> end()) {
    std::set<int> existingIDs = entityIter -> second; 
    std::set<int>::iterator it = existingIDs.find(id);
    // if id does not exist
    if(it == existingIDs.end()) {
      // insert and update
      existingIDs.insert(id);
      entityIter -> second = existingIDs;
    }
  } else {
    // if entity does not exist
    // create set of ID, insert, and add to the map
    std::set<int> existingIDs;
    existingIDs.insert(id);
    entity_id_map_->insert(std::pair<std::string,std::set<int>>(entity, existingIDs));
  }
}

bool RequestHandlerAPI::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Serving API Request";

  // check request method
  auto method_sv = req.method_string();
  std::string method(method_sv);
  // declare variables
  int id = 0;
  // full local path
  fs::path full_path;
  std::string content_type;
  std::string response_body = "";
  // entity name (eg Shoes)
  std::string entity;
  // parse url path, set id and entity
  if(!parse_url_target(req, full_path, id, entity)) {
    BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
    set_notfound_request(res);
    return false;
  }
  // Create
  if (method == "POST") {
    // returns 400 full_path not 1 level lower than root
    if (full_path.parent_path() != base_path) {
      BOOST_LOG_TRIVIAL(error) << "API posting: bad target: " << full_path.string() << std::endl;
      set_bad_request(res);
      return false;
    }
    // create entity if not already exists
    if (!file->exists(full_path)) {
      file->create_directory(full_path);
      std::set<int> newIDs;
      entity_id_map_->insert(std::pair<std::string,std::set<int>>(entity, newIDs));
    }
    // get available id and create file
    id = get_next_id(entity);
    full_path /= std::to_string(id);
    // create entity file
    file->write_file(full_path, req.body());
    // construct 201 Created message
    res.result(http::status::created);
    res.body() = "{\"id\": " + std::to_string(id) + "}";
    res.prepare_payload();
    BOOST_LOG_TRIVIAL(info) << "API posting " << full_path.string() << std::endl;
  } else if (method == "GET") {
    // Retrieve or List
    // List
    if (id == 0) {
      // path not found, return 404
      if (!file->exists(full_path)) {
        BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
        set_notfound_request(res);
        return false;
      } 
      std::string response  = "[";
      std::map<std::string, std::set<int>>::iterator it = entity_id_map_->find(entity);  
      // if entity exists
      if (it != entity_id_map_->end()) {
        // go over IDs and add to response
			  std::set<int> IDs = it -> second; 
        for(int ID:IDs) {
          response += std::to_string(ID) + ",";
        }
        // removing the trailing ','
        response = response.substr(0, response.length()-1);
        response += "]";
		  } else {
        // entity does not exists in the map
        response = "Entity: " + entity + " is currently empty!"; 
      }
      // set content type
      content_type = "text/plain";
      response_body = response;
      BOOST_LOG_TRIVIAL(info) << "API listing " << full_path.string() << std::endl;
    } else {
      // Retrive 
      // file not found, return 404
      if (!file->exists(full_path) || !file->is_regular_file(full_path)) {
        BOOST_LOG_TRIVIAL(error) << "404 Not Found: " << full_path.string();
        set_notfound_request(res);
        erase_if_exist(entity, id);
        return false;
      } else {
        // file found, get file
        // get content type
        content_type = extensionToMIME(full_path.extension().string());
        // get file contents
        response_body = file->read_file(full_path);
        insert_if_notexist(entity, id);
        BOOST_LOG_TRIVIAL(info) << "API Retrieving " << full_path.string();
      }
    }
    // fill in response
    res.result(http::status::ok);
    res.set(http::field::content_type, content_type);
    res.body() = response_body;
    res.prepare_payload();
  } else if (method == "PUT") {
    // Update 
    // return 400 if id not specified or full_path's grandparent folder is not base path
    if (id == 0 || full_path.parent_path().parent_path() != base_path) {
      BOOST_LOG_TRIVIAL(error) << "API PUT: path not supported " << full_path.string();
      set_bad_request(res);
      return false;
    } else {
      // create entity if not already exists
      if (!file->exists(full_path.parent_path())) {
        file->create_directory(full_path.parent_path());
      }
      // write to entity file
      file->write_file(full_path, req.body());
      // construct 200 ok message
      res.result(http::status::ok);
      res.prepare_payload();
      insert_if_notexist(entity, id);
      BOOST_LOG_TRIVIAL(info) << "API putting " << full_path.string() << std::endl;
    }
  } else if (method == "DELETE") {
    // Delete
    // file not found or not regular file, returns 404 (not found)
    if (!file->exists(full_path) || !file->is_regular_file(full_path)) {
      BOOST_LOG_TRIVIAL(error) << "API DELETE: not found for " << full_path.string();
      set_notfound_request(res);
      return false;
    } else {
      file->remove(full_path);
      res.result(http::status::ok);
      BOOST_LOG_TRIVIAL(error) << "API DELETE: removed " << full_path.string();
      res.prepare_payload();
    }
    erase_if_exist(entity, id);
  } else if (method == "HEAD" || method == "CONNECT" || method == "OPTIONS" ||
      method == "TRACE" || method == "PATCH") {
    // Unsupported method
    BOOST_LOG_TRIVIAL(error) << "Received Unsupported Request";
    res.result(http::status::method_not_allowed);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>405 Not Allowed</h1></html>";
    res.prepare_payload();
    return false;
  } else {
    // Bad Request
    BOOST_LOG_TRIVIAL(error) << "Received Bad Request";
    set_bad_request(res);
    return false;
  }

  BOOST_LOG_TRIVIAL(info) << "Created Response: " << std::to_string(res.result_int());
  return true;
}

bool RequestHandlerAPI::parse_url_target(const http::request<http::string_body> req, 
    boost::filesystem::path& full_path, int& id, std::string& entity) {
  // construct full file path
  auto url_sv = req.target();
  std::string url(url_sv);
  // this is here to deprecate prefix_length_ in future change
  int prefix_length = prefix_length_;
  // url should be something like /Shoes/1
  url = url.substr(prefix_length);
  BOOST_LOG_TRIVIAL(debug) << "CURD url is" << url;
  // remove the leading /
  if(url[0] != '/') {
    BOOST_LOG_TRIVIAL(warning) << "CURD url entity not starting with /";
    id = 0;
    entity = "";
    return false;
  }
  // remove the trailing / if exists
  if(url.back() == '/') {
    BOOST_LOG_TRIVIAL(debug) << "CURD removing trailing slash..." << entity;
    url = url.substr(0, url.length()-1);
  }
  full_path = base_path;
  full_path /= url;
  //full path is not /api/Shoes/1
  // get the request id (if any)
  size_t pos = url.find_last_of('/');
  if (pos == std::string::npos) { // "/" not existed in path, we only got entity
    // BOOST_LOG_TRIVIAL(warning) << "CURD url entity not exists";
    id = 0;
    entity = url;
    return false;
  } else { // we have entity and ID
    try {
      id = std::stoi(url.substr(pos + 1)); // convert id to int
      entity = url.substr(1, pos - 1);
    }
    catch(std::exception &err) { // path not ended with number
      id = 0;
      entity = url.substr(1);
    }
  }
  // parse success
  return true;
}

bool RequestHandlerSleep::get_response(const http::request<http::string_body> req,
                                     http::response<http::string_body>& res) {
    BOOST_LOG_TRIVIAL(info) << "Sleep Handler Blocking";

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    // 200 OK
    res.version(11);
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/html");
    res.body() = "<html><h1>Slept for 1 second</h1></html>";
    // Fill in content length
    res.prepare_payload();

    BOOST_LOG_TRIVIAL(info) << "Created Response: 200 OK";
    return true;
}

// handles request req by filling in response res with contents of
// target file or error message, returns true to indicate 200 response
// returns false to indicate 400, 404 or 405 response
bool RequestHandlerLogin::get_response(const http::request<http::string_body> req, 
    http::response<http::string_body>& res) {
  BOOST_LOG_TRIVIAL(info) << "Serving Login Request";

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
  // Note that a lot of this code is repeated from the static handler
  // However, because we deal with the URL differently, the code is copied here instead of
  // being part of the parent class since other types of handlers should not have
  // access to a function performing the serving below
  boost::filesystem::path full_path = base_path;
  full_path /= login_path;

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