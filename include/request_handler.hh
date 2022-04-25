#include <string>
#include <boost/filesystem.hpp>

class RequestHandler {
  public:
    virtual std::string get_response(std::string request_str) = 0;
};

class RequestHandlerEcho : public RequestHandler {
  public:
    RequestHandlerEcho();
    std::string get_response(std::string request_str);
};

class RequestHandlerStatic : public RequestHandler {
  public:
    RequestHandlerStatic(std::string serve_path) : base_path(serve_path) {};
    std::string get_response(std::string request_str);
  private:
    boost::filesystem::path base_path;
};
