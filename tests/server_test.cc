#include "gtest/gtest.h"
#include "server.hh"
#include "session.hh"
#include <boost/system/error_code.hpp>

class ServerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ;
  }

  boost::asio::io_service io;

  // Avoid port 80 for testing
  int port = 8080;
  server s = server(io, port);
};

/*TEST_F(ServerTest, SuccessfulStartAccept) {
  int success = s.test_start_accept();
  EXPECT_EQ(success, 1);
}

TEST_F(ServerTest, SuccessfulHandleAccept) {
  boost::asio::io_service serv;
  session* sess = new session(serv, routes);
  int success = s.test_handle_accept(sess, boost::system::error_code());

  if (sess)
    delete sess;
  EXPECT_EQ(success, 1);
}

TEST_F(ServerTest, FailedHandleAccept) {
  boost::asio::io_service serv;
  session* sess = new session(serv, routes);
  int success = s.test_handle_accept(sess,
    boost::system::errc::make_error_code(boost::system::errc::not_supported));

  if (sess)
    delete sess;
  EXPECT_EQ(success, -1);
} */

TEST_F(ServerTest, RoutesSmokeTest) {
  NginxConfigParser config_parser;
  NginxConfig config;
  config_parser.Parse("request_handlers_config", &config);
  server::generate_routes(&config);
  std::map<std::string, RequestHandlerFactory*> routes = server::get_routes();

  EXPECT_EQ(routes.count("/echo"), 1);
  EXPECT_EQ(routes.count("/static"), 1);
  EXPECT_EQ(routes.size(), 2);
}