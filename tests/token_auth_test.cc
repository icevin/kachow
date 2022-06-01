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

TEST_F(TokenAuthTest, TokenAuthPrivate1) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, PRIVATE_AUTH);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, TokenAuthPrivate2) {
  std::string iss = "https://accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, PRIVATE_AUTH);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, TokenAuthPublic1) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@gmail.com";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, PUBLIC_AUTH);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, TokenAuthPublic2) {
  std::string iss = "https://accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@gmail.com";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, PUBLIC_AUTH);
  EXPECT_EQ(s, sub);
}

TEST_F(TokenAuthTest, InvalidIssuer) {
  std::string iss = "accounts.bing.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, NO_AUTH);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidAudience) {
  std::string iss = "accounts.google.com";
  std::string aud = "test456.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string sub = "12345678";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, NO_AUTH);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidSignature) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "tset-562-tib-terces";
  std::string sub = "12345678";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_subject(sub)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, NO_AUTH);
  EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, NoSubject) {
  std::string iss = "accounts.google.com";
  std::string aud = "test123.apps.googleusercontent.com";
  std::string secret = "test-256-bit-secret";
  std::string email = "test@g.ucla.edu";
  auto token = jwt::create()
    .set_issuer(iss)
    .set_audience(aud)
    .set_payload_claim("email", jwt::claim(email))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{secret});

  auth_level lvl = t->get_auth(token, s);
  EXPECT_EQ(lvl, NO_AUTH);
  EXPECT_EQ(s, "init");
}
