#include "server.hh"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <iostream>

#include "session.hh"
#include "token_auth.hh"

using boost::asio::ip::tcp;

std::map<std::string, RequestHandlerFactory*> server::routes_;

server::server(boost::asio::io_service& io_service, int port,
	const char* pk_file, const char* pem_file, int num_threads)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      number_threads(num_threads),
      context_(boost::asio::ssl::context::sslv23) {
    context_.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::sslv23
        | boost::asio::ssl::context::single_dh_use);
    context_.use_certificate_chain_file(pem_file);
    context_.use_private_key_file(pk_file, boost::asio::ssl::context::pem);
    BOOST_LOG_TRIVIAL(info) << "Server class constructed";
}

void server::run() {
    start_accept();
}

int server::start_accept() {
    session* new_session = new session(io_service_, context_, server::routes_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));

    std::vector<boost::shared_ptr<boost::thread>> thread_pool;

    for (int i = 0; i < number_threads; i++) {
        boost::thread* forked_thread = new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_));
        boost::shared_ptr<boost::thread> thread_ptr(forked_thread);
        thread_pool.push_back(thread_ptr);
    }

    for (boost::shared_ptr<boost::thread> thread_ptr : thread_pool) {
        thread_ptr->join();
    }
    
    return 1;
}

int server::handle_accept(session* new_session,
                          const boost::system::error_code& error) {
    BOOST_LOG_TRIVIAL(info) << "Server: handle accept";
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
void server::generate_routes(NginxConfig* config, FileSystem* fs) {
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
                    if (name == "APIHandler") {
                        std::string root_file_path = sub_statement->child_block_->statements_[0]->tokens_[1];
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(
                            location, new APIHandlerFactory(root_file_path, fs)));
                    }
                    if (name == "SecureAPIHandler") {
                        if (sub_statement->child_block_->statements_.size() < 2) {
                            BOOST_LOG_TRIVIAL(fatal) << "Incorrect arguments passed to SecureAPIHandler\n"
                                                       "Should be: root_file_path <path>;\n"
                                                       "           auth_type <OAUTH/FAKE> (<client_id> <public_key_path>)\n";
                        }
                        std::string root_file_path = sub_statement->child_block_->statements_[0]->tokens_[1];
                        std::string auth_type_str = sub_statement->child_block_->statements_[1]->tokens_[1];
                        Auth* authorizer;
                        if (auth_type_str == "OAUTH") {
                            std::string client_id = sub_statement->child_block_->statements_[1]->tokens_[2];
                            std::string public_key_path = sub_statement->child_block_->statements_[1]->tokens_[3];
                            const std::ifstream input_stream(public_key_path, std::ios_base::binary);

                            if (input_stream.fail()) {
                                BOOST_LOG_TRIVIAL(fatal) << "Invalid public_key_path provided";
                            }

                            authorizer = new TokenAuth(client_id, public_key_path);
                        } else if (auth_type_str == "FAKE") {
                            authorizer = new FakeAuth();
                        } else {
                            BOOST_LOG_TRIVIAL(fatal) << "Invalid Auth Type: Must be OAUTH or FAKE\n";
                        }
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(
                            location, new SecureAPIHandlerFactory(root_file_path, fs, authorizer)));
                    }
                    if (name == "HealthHandler") {
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(location, new HealthHandlerFactory()));
                    }
                    if (name == "SleepHandler") {
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(location, new SleepHandlerFactory()));
                    }
                    if (name == "LoginHandler") {
                        std::string root_file_path = sub_statement->child_block_->statements_[0]->tokens_[1];
                        routes.insert(std::pair<std::string, RequestHandlerFactory*>(
                            location, new LoginHandlerFactory(root_file_path)));
                    }
                }
            }
        }
    }
    // printing map (for debugging)
    std::map<std::string, RequestHandlerFactory*>::iterator itr;
    BOOST_LOG_TRIVIAL(debug) << "\nThe map Server::routes is : \n";
    for (itr = routes.begin(); itr != routes.end(); ++itr) {
        BOOST_LOG_TRIVIAL(debug) << '\t' << itr->first << '\t' << itr->second
                                 << '\n';
    }

    routes_ = routes;
}

std::map<std::string, RequestHandlerFactory*> server::get_routes() {
    return routes_;
}
