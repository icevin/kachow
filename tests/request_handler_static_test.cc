#include "gtest/gtest.h"
#include "request_handler.hh"

#include <iostream>

using namespace std;

class StaticTest : public ::testing::Test {
 protected:


};

TEST_F(StaticTest, SuccessfulStart) {
  RequestHandlerStatic a("");
  cout << a.get_response("GET /small.png HTTP/1.1") << endl;
}