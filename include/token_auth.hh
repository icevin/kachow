#pragma once

#include <string>

typedef enum {
  NO_AUTH,
  PUBLIC_AUTH,
  PRIVATE_AUTH
} auth_level;

class TokenAuth {
 public:
  TokenAuth(std::string client_id, std::string client_secret) 
    : client_id_(client_id), client_secret_{client_secret} {};
  auth_level get_auth(std::string id_token, std::string& user_id);
  
 private:
  std::string client_id_;
  std::string client_secret_;
};
