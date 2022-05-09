#include "server.hh"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>

#include "session.hh"

using boost::asio::ip::tcp;

std::map<std::string, RequestHandlerFactory*> server::routes_;

server::server(boost::asio::io_service& io_service, int port, std::vector<std::vector<std::string>> handler_statements)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
    handler_statements_ = handler_statements;
    start_accept();
}

int server::start_accept() {
    session* new_session = new session(io_service_, server::routes_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
    return 1;
}

int server::handle_accept(session* new_session,
                          const boost::system::error_code& error) {
    int ret = 0;
    if (!error) {
        new_session->start();
        ret = 1;
    } else {
        delete new_session;
        ret = -1;
    }

    start_accept();
    return ret;
}

int server::test_start_accept() {
    return start_accept();
}

int server::test_handle_accept(session* new_session,
                               const boost::system::error_code& error) {
    return handle_accept(new_session, error);
}

// Creates a map from locations (e.g. /static or /echo )
// to RequestHandlerFactory pointers
void server::generate_routes(NginxConfig* config) {
    std::map<std::string, RequestHandlerFactory*> routes;
    for (const auto& statement : config->statements_) {
        if (statement->tokens_[0] == "server") {
            for (const auto& sub_statement : statement->child_block_->statements_) {
                if (sub_statement->tokens_[0] == "location") {
                    std::string location = sub_statement->tokens_[1];
                    std::string name     = sub_statement->tokens_[2];
                    if (name == "EchoHandler")
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(
                            location, new EchoHandlerFactory()));
                    if (name == "StaticHandler") {
                        std::string root_file_path = sub_statement->child_block_->statements_[0]->tokens_[1];
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(
                            location, new StaticHandlerFactory(root_file_path)));
                    }
                    if (name == "NotFoundHandler") {
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(location, new NotFoundHandlerFactory()));
                    }
                }
            }
        }
    }
    // printing map (for debugging)
    std::map<std::string, RequestHandlerFactory*>::iterator itr;
    BOOST_LOG_TRIVIAL(info) << "\nThe map Server::routes is : \n";
    for (itr = routes.begin(); itr != routes.end(); ++itr) {
        BOOST_LOG_TRIVIAL(info) << '\t' << itr->first << '\t' << itr->second
                                << '\n';
    }

    routes_ = routes;
}

std::map<std::string, RequestHandlerFactory*> server::get_routes() {
    return routes_;
}