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
    std::vector<std::vector<std::string>> handler_statements;
    s = new session(io, handler_statements);
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

TEST_F(SessionTest, PrefixMatcherTrueWhenNeeded) {
  std::string target = "/echo/stuff";
  std::string prefix = "/echo";
  bool result = s->url_prefix_matches(target, prefix);
  std::cout << "PrefixMatcherTrueWhenNeeded result: " << result << "\n";
  EXPECT_TRUE(result);
}

TEST_F(SessionTest, PrefixMatcherFalseWhenNeeded) {
  std::string target = "/static/stuff";
  std::string prefix = "/echo";
  int result = s->url_prefix_matches(target, prefix);
  std::cout << "PrefixMatcherFalseWhenNeeded result: " << result << "\n";
  EXPECT_FALSE(result);
}

TEST_F(SessionTest, PrefixMatcherTrueWhenSlash) {
  std::string target = "/foo/bar";
  std::string prefix = "/";
  int result = s->url_prefix_matches(target, prefix);
  std::cout << "PrefixMatcherTrueWhenSlash result: " << result << "\n";
  EXPECT_TRUE(result);
}