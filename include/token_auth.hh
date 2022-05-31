#pragma once

#include <string>

class TokenAuth {
 public:
  TokenAuth(std::string client_id, std::string client_secret) 
    : client_id_(client_id), client_secret_{client_secret} {};
  bool get_user_id(std::string id_token, std::string& user_id);
  
 private:
  std::string client_id_;
  std::string client_secret_;
};
