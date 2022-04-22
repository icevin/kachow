#include <string>

class RequestHandler
{
    public:
        virtual std::string get_response(std::string request_str) = 0;
};

class RequestHandlerEcho: public RequestHandler {
   public:
      std::string get_response(std::string request_str);
};