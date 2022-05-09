#include "gtest/gtest.h"
#include "request_handler.hh"

#include <iostream>
#include <string>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using namespace std;

class EchoTest : public ::testing::Test {
 protected:
  RequestHandlerEcho* echo_handler;

  void SetUp() override {
    echo_handler = new RequestHandlerEcho();
  }

  void TearDown() override {
    delete echo_handler;
  }

};

class StaticTest : public ::testing::Test {
 protected:
  RequestHandlerStatic* static_handler;

  void SetUp() override {
    static_handler = new RequestHandlerStatic(".", 0);
  }

  void TearDown() override {
    delete static_handler;
  }

};

TEST_F(EchoTest, SuccessfulEcho) {
  http::request<http::string_body> req(http::verb::get, "/echo", 11);
  http::response<http::string_body> res;

  bool status = echo_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
  EXPECT_EQ(res.body(), "GET /echo HTTP/1.1\r\n\r\n");
}

TEST_F(StaticTest, Static200) {
  http::request<http::string_body> req(http::verb::get, "/invalid_config", 11);
  http::response<http::string_body> res;

  bool status = static_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
  EXPECT_EQ(res.body(), "invalid config");
}

TEST_F(StaticTest, Static400) {
  http::request<http::string_body> req;
  cout << req.method_string();
  http::response<http::string_body> res;

  bool status = static_handler->get_response(req, res);
  
  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 400);
  EXPECT_EQ(res.body(), "<html><h1>400 Bad Request</h1></html>");
}

TEST_F(StaticTest, Static404) {
  http::request<http::string_body> req(http::verb::get, "/non_existing_file", 11);
  http::response<http::string_body> res;

  bool status = static_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 404);
  EXPECT_EQ(res.body(), "<html><h1>404 Not Found</h1></html>");
}

TEST_F(StaticTest, Static405) {
  http::request<http::string_body> req(http::verb::post, "/bad", 11);
  http::response<http::string_body> res;

  bool status = static_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 405);
  EXPECT_EQ(res.body(), "<html><h1>405 Not Allowed</h1></html>");
}