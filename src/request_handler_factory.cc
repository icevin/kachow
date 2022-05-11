#include "request_handler.hh"
#include "request_handler_factory.hh"
#include <string>

// Create a new RequestHandler object using the factory
RequestHandler* StaticHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerStatic(this->root_file_path_, location.length());
}

RequestHandler* EchoHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerEcho();
}

RequestHandler* NotFoundHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerNotFound();
}

RequestHandler* APIHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerAPI(this->root_file_path_, location.length());
}