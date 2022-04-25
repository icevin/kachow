#include "gtest/gtest.h"
#include "config_parser.hh"
#include <vector>

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

TEST_F(NginxConfigParserTest, ValidPortNumberAndComment) {
  bool success = parser.Parse("comment_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 8080);
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

TEST_F(NginxConfigParserTest, MatchedDoubleQuoteInvalidFollowing) {
  bool success = parser.Parse("matched_dquote_invalid_following", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
}

TEST_F(NginxConfigParserTest, MatchedSingleQuoteConfig) {
  bool success = parser.Parse("matched_squote_config", &out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(out_config.portNumber(), 80);
}

TEST_F(NginxConfigParserTest, MatchedSingleQuoteInvalidFollowing) {
  bool success = parser.Parse("matched_squote_invalid_following", &out_config);
  EXPECT_FALSE(success);
  EXPECT_EQ(out_config.portNumber(), -1);
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

TEST_F(NginxConfigParserTest, ConfigStatementToStringTestDepth0) {
  std::istringstream test("server {\nlisten   80;\n}");
  std::istream& input(test);
  bool success = parser.Parse(&input, &out_config);
  std::string expected_output_0 = "server {\n  listen 80;\n}\n";
  EXPECT_EQ(out_config.ToString(0), expected_output_0);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigStatementToStringTestDepth1) {
  std::istringstream test("server {\nlisten   80;\n}");
  std::istream& input(test);
  bool success = parser.Parse(&input, &out_config);
  std::string expected_output_1 = "  server {\n    listen 80;\n  }\n";
  EXPECT_EQ(out_config.ToString(1), expected_output_1);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, RequestHandlerStatements) {
  bool success = parser.Parse("request_handlers_config", &out_config);
  EXPECT_TRUE(success);
  std::vector<std::vector<std::string>> handler_statements = out_config.getRequestHandlerStatements();
  EXPECT_EQ(handler_statements[0][0], "static_serve");
  EXPECT_EQ(handler_statements[0][1], "/static");
  EXPECT_EQ(handler_statements[0][2], ".");
  EXPECT_EQ(handler_statements[1][0], "echo");
  EXPECT_EQ(handler_statements[1][1], "/echo");
}