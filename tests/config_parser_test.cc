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

TEST_F(NginxConfigParserTest, ValidPortNumber) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 80);
}

TEST_F(NginxConfigParserTest, InvalidPortNumber) {
  bool success = parser.Parse("invalid_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, EmptyConfig) {
  bool success = parser.Parse("empty_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, ExtraRBraceConfig) {
  bool success = parser.Parse("extra_rbrace_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, ExtraLBraceConfig) {
  bool success = parser.Parse("extra_lbrace_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, MatchedDoubleQuoteConfig) {
  bool success = parser.Parse("matched_dquote_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 80);
}

TEST_F(NginxConfigParserTest, MatchedSingleQuoteConfig) {
  bool success = parser.Parse("matched_squote_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 80);
}

// " with '
TEST_F(NginxConfigParserTest, MismatchedQuoteConfig) {
  bool success = parser.Parse("mismatched_quote_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, UnmatchedDoubleQuoteConfig) {
  bool success = parser.Parse("unmatched_dquote_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, UnmatchedSingleQuoteConfig) {
  bool success = parser.Parse("unmatched_squote_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, ExtraBracesConfig) {
  bool success = parser.Parse("extra_braces_config", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, FalseFileConfig) {
  bool success = parser.Parse("config", &out_config);
  EXPECT_FALSE(success);
}