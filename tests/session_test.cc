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
    s = new session(io);
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

TEST_F(SessionTest, SuccessfulHandleRead) {
  int rc = s->test_handle_read(boost::system::error_code(), 0);
  EXPECT_EQ(rc, 1);
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