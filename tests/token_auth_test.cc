#include "gtest/gtest.h"
#include "token_auth.hh"

#include <string>
#include <jwt-cpp/jwt.h>

class TokenAuthTest : public ::testing::Test {
 protected:
  std::string s;
  TokenAuth* t;

  void SetUp() override {
    s = "init";
    t = new TokenAuth("test123.apps.googleusercontent.com", "test-256-bit-secret");
  }

  void TearDown() override {
      delete t;
  }

};

TEST_F(TokenAuthTest, TokenAuthSuccess1) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_TRUE(rc);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, TokenAuthSuccess2) {
  std::string iss = "https://accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_TRUE(rc);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, InvalidIssuer) {
  std::string iss = "accounts.bing.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_FALSE(rc);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidAudience) {
  std::string iss = "accounts.google.com";
  std::string aud = "test456.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_FALSE(rc);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidSignature) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "tset-562-tib-terces";
  std::string sub = "12345678";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_FALSE(rc);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, NoSubject) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  bool rc = t->get_user_id(token, s);
  EXPECT_FALSE(rc);
  EXPECT_EQ(s, "init");
}
