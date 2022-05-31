#include "token_auth.hh"

#include <string>
#include <jwt-cpp/jwt.h>

// verifies 'id_token' and gets the user id
// user id is stored in 'user_id' string parameter, pass in by reference
// returns true if token has valid signature, correct audience and issuer
// returns false if any of these checks fail
bool TokenAuth::get_user_id(std::string id_token, std::string& user_id) {
  try {
    // decode token
    auto decoded_token = jwt::decode(id_token);

    // verify token signature
    auto verifier = jwt::verify()
      .allow_algorithm(jwt::algorithm::hs256{ client_secret_ });
    verifier.verify(decoded_token);

    // check token audience
    auto aud = decoded_token.get_audience();
    if (aud.find(client_id_) == aud.end()) {
      // client id not found in audience
      return false;
    }

   // check token issuer
    auto iss = decoded_token.get_issuer();
    std::string google1 = "accounts.google.com";
    std::string google2 = "https://accounts.google.com";
    if (iss != google1 && iss != google2) {
      // Google is not the issuer
      return false;
    }

    // get user id
    user_id = decoded_token.get_subject();       
    return true;
  } 
  catch (std::exception& e) {
    // verifier throws exeption if token signature is invalid
    return false;
  }
}
