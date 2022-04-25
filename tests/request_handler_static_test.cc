#include "gtest/gtest.h"
#include "request_handler.hh"

#include <iostream>
#include <string>

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
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 14\r\n\r\ninvalid config";
  EXPECT_EQ(expected_response, a->get_response("GET /invalid_config HTTP/1.1"));
}

TEST_F(StaticTest, Static400) {
  std::string expected_response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 37\r\n\r\n<html><h1>400 Bad Request</h1></html>";
  EXPECT_EQ(expected_response, a->get_response("INVALID HTTP REQUEST"));
}

TEST_F(StaticTest, Static404) {
  std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 35\r\n\r\n<html><h1>404 Not Found</h1></html>";
  EXPECT_EQ(expected_response, a->get_response("GET /non_existing_file HTTP/1.1"));
}