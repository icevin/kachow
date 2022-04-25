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
    RequestHandlerStatic(std::string serve_path, int prefix_length) : base_path(serve_path), prefix_length_{prefix_length} {};
    std::string get_response(std::string request_str);
  private:
    boost::filesystem::path base_path;
    int prefix_length_;
};
