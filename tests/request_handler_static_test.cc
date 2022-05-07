#include "gtest/gtest.h"
#include "request_handler.hh"

#include <iostream>
#include <string>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using namespace std;

class StaticTest : public ::testing::Test {
 protected:
  RequestHandlerStatic* a;

  void SetUp() override {
    a = new RequestHandlerStatic(".", 0);
  }

  void TearDown() override {
    delete a;
  }

};

TEST_F(StaticTest, Static200) {
  http::request<http::string_body> req(http::verb::get, "/invalid_config", 11);
  http::response<http::string_body> res;

  a->get_response(req, res);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(StaticTest, Static400) {
  http::request<http::string_body> req(http::verb::post, "/bad", 11);
  http::response<http::string_body> res;

  a->get_response(req, res);
  EXPECT_EQ(res.result_int(), 400);
}

TEST_F(StaticTest, Static404) {
  http::request<http::string_body> req(http::verb::get, "/non_existing_file", 11);
  http::response<http::string_body> res;

  a->get_response(req, res);
  EXPECT_EQ(res.result_int(), 404);
}