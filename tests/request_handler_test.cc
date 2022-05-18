#include "gtest/gtest.h"
#include "request_handler.hh"

#include <iostream>
#include <string>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using namespace std;

class EchoTest:public::testing::Test {
  protected:
    RequestHandlerEcho* echo_handler;

    void SetUp() override {
      echo_handler = new RequestHandlerEcho();
    }

    void TearDown() override {
      delete echo_handler;
    }
};

class StaticTest:public::testing::Test {
  protected:
    RequestHandlerStatic* static_handler;

    void SetUp() override {
      static_handler = new RequestHandlerStatic(".", 0);
    }

    void TearDown() override {
      delete static_handler;
    }
};

class NotFoundTest:public::testing::Test {
  protected:
    RequestHandlerNotFound* not_found_handler;

    void SetUp() override {
      not_found_handler = new RequestHandlerNotFound();
    }

    void TearDown() override {
      delete not_found_handler;
    }
};

class APITest:public::testing::Test {
  protected:
    RequestHandlerAPI* API_handler;

    void SetUp() override {
      mock_id_map = new std::map<std::string, std::set<int>>;
      fs = new FakeFileSsystem();
      API_handler = new RequestHandlerAPI(".", 0, mock_id_map, fs);
    }

    void TearDown() override {
      delete mock_id_map;
      delete fs;
      delete API_handler;
    }

    std::map<std::string, std::set<int>>* mock_id_map;
    FileSystem* fs;
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

TEST_F(NotFoundTest, NotFound) {
  http::request<http::string_body> req(http::verb::get, "/", 11);
  http::response<http::string_body> res;

  bool status = not_found_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 404);
  EXPECT_EQ(res.body(), "<html><h1>404 Not Found</h1></html>");
}

TEST_F(APITest, POSTSuccessTest) {
  http::request<http::string_body> req(http::verb::post, "/test", 11);
  http::response<http::string_body> res;

  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 201);
  EXPECT_EQ(res.body(), "{\"id\": 1}");
}

TEST_F(APITest, POSTFailTest) {
  http::request<http::string_body> req(http::verb::post, "", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 404);
}

TEST_F(APITest, POSTMultipleLevelTest) {
  http::request<http::string_body> req(http::verb::post, "/test/next", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 400);
}

TEST_F(APITest, GETPathNotFoundTest) {
  http::request<http::string_body> req(http::verb::get, "/test/0", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 404);
}

TEST_F(APITest, GETFileNotFoundTest) {
  http::request<http::string_body> req(http::verb::get, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 404);
}

TEST_F(APITest, GETSuccessTest) {
  http::request<http::string_body> req_post(http::verb::post, "/test", 11);
  http::response<http::string_body> res_post;
  API_handler->get_response(req_post, res_post);

  http::request<http::string_body> req(http::verb::get, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(APITest, GETSuccessTest2) {
  http::request<http::string_body> req_post1(http::verb::post, "/test", 11);
  http::response<http::string_body> res_post1;
  API_handler->get_response(req_post1, res_post1);

  http::request<http::string_body> req(http::verb::get, "/test/", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(APITest, PUTCreateTest) {
  http::request<http::string_body> req(http::verb::put, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(APITest, PUTNotFoundTest) {
  http::request<http::string_body> req(http::verb::put, "/test/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 400);
}

TEST_F(APITest, PUTSuccessFoundTest) {
  http::request<http::string_body> req_post(http::verb::post, "/test", 11);
  http::response<http::string_body> res_post;
  API_handler->get_response(req_post, res_post);

  http::request<http::string_body> req(http::verb::put, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(APITest, DELETENotFoundTest) {
  http::request<http::string_body> req(http::verb::delete_, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 404);
}

TEST_F(APITest, DELETESuccessFoundTest) {
  http::request<http::string_body> req_post(http::verb::post, "/test", 11);
  http::response<http::string_body> res_post;
  API_handler->get_response(req_post, res_post);

  http::request<http::string_body> req(http::verb::delete_, "/test/1", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_TRUE(status);
  EXPECT_EQ(res.result_int(), 200);
}

TEST_F(APITest, NotAllowTest) {
  http::request<http::string_body> req(http::verb::head, "/test", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 405);
  EXPECT_EQ(res.body(), "<html><h1>405 Not Allowed</h1></html>");
}

TEST_F(APITest, BadAPIRequestTest) {
  http::request<http::string_body> req(http::verb::mkcalendar, "/test", 11);
  http::response<http::string_body> res;
  bool status = API_handler->get_response(req, res);

  EXPECT_FALSE(status);
  EXPECT_EQ(res.result_int(), 400);
  EXPECT_EQ(res.body(), "<html><h1>400 Bad Request</h1></html>");
}