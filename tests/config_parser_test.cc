#include "gtest/gtest.h"
#include "config_parser.hh"

class NginxConfigParserTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ;
  }

  NginxConfigParser parser;
  NginxConfig out_config;
};

TEST_F(NginxConfigParserTest, PortNumber) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 80);
}