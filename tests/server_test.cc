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
  std::vector<std::vector<std::string>> handler_statements;
  //std::vector<std::string> handler_statement;
  //handler_statement.push_back("echo");
  //handler_statement.push_back("/");
  //handler_statements.push_back(handler_statement);
  server s = server(io, port, handler_statements);
};

TEST_F(ServerTest, SuccessfulStartAccept) {
  int success = s.test_start_accept();
  EXPECT_EQ(success, 1);
}

TEST_F(ServerTest, SuccessfulHandleAccept) {
  boost::asio::io_service serv;
  session* sess = new session(serv, handler_statements);
  int success = s.test_handle_accept(sess, boost::system::error_code());

  if (sess)
    delete sess;
  EXPECT_EQ(success, 1);
}

TEST_F(ServerTest, FailedHandleAccept) {
  boost::asio::io_service serv;
  session* sess = new session(serv, handler_statements);
  int success = s.test_handle_accept(sess,
    boost::system::errc::make_error_code(boost::system::errc::not_supported));

  if (sess)
    delete sess;
  EXPECT_EQ(success, -1);
}