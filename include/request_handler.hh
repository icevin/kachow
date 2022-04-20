#include <string>

class RequestHandler
{
    public:
        virtual std::string get_response(std::string request_str) = 0;
};