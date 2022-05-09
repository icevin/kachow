#include "gtest/gtest.h"
#include "server.hh"
#include "session.hh"

#include <boost/system/error_code.hpp>

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
 protected:
  boost::asio::io_service io;
  session* s;

  void SetUp() override {
    std::map<std::string, RequestHandlerFactory*> routes;
    s = new session(io, routes);
  }

};

class MatchTest : public ::testing::Test {
 protected:
  boost::asio::io_service io;
  session* s;
  std::map<std::string, RequestHandlerFactory*> routes;
  EchoHandlerFactory* echo_factory;
  StaticHandlerFactory* static_factory;
  NotFoundHandlerFactory* not_found_factory;

  void SetUp() override {
    echo_factory = new EchoHandlerFactory();
    static_factory = new StaticHandlerFactory("./");
    not_found_factory = new NotFoundHandlerFactory();
    routes.insert(std::pair<std::string, RequestHandlerFactory*>("/hello", echo_factory));
    routes.insert(std::pair<std::string, RequestHandlerFactory*>("/hello/world", static_factory));
    routes.insert(std::pair<std::string, RequestHandlerFactory*>("/hi", not_found_factory));
    s = new session(io, routes);
  }

  void TearDown() override {
    delete echo_factory;
    delete static_factory;
    delete not_found_factory;
  }

};

TEST_F(SessionTest, SuccessfulStart) {
  int rc = s->test_start();
  EXPECT_EQ(rc, 1);
  delete s;
}

TEST_F(SessionTest, SuccessfulSocket) {
  EXPECT_EQ(io.get_executor(), s->socket().get_executor());
  delete s;
}

TEST_F(SessionTest, SuccessfulHandleWrite) {
  int rc = s->test_handle_write(boost::system::error_code());
  EXPECT_EQ(rc, 1);
  delete s;
}

TEST_F(SessionTest, FailedHandleRead) {
  int rc = s->test_handle_read(boost::system::errc::make_error_code(boost::system::errc::not_supported), 0);

  EXPECT_EQ(rc, -1);
}

TEST_F(SessionTest, FailedHandleWrite) {
  int rc = s->test_handle_write(boost::system::errc::make_error_code(boost::system::errc::not_supported));

  EXPECT_EQ(rc, -1);
}

TEST_F(MatchTest, CompleteMatch) {
  std::string req_url = "/hi";
  std::string location = session::match(routes, req_url);
  EXPECT_EQ(location, "/hi");
}

TEST_F(MatchTest, PrefixMatch) {
  std::string req_url = "/hi/there";
  std::string location = session::match(routes, req_url);
  EXPECT_EQ(location, "/hi");
}

TEST_F(MatchTest, LongestMatch) {
  std::string req_url = "/hello/world";
  std::string location = session::match(routes, req_url);
  EXPECT_EQ(location, "/hello/world");
}
