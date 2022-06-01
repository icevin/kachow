#include "token_auth.hh"

#include <string>
#include <jwt-cpp/jwt.h>

// returns the authorization level of a given 'id_token'
//   - NO_AUTH if token has invalid signature, incorrect or missing claims
//   - PUBLIC_AUTH if token is valid and email domain is NOT 'g.ucla.edu'
//   - PRIVATE_AUTH if token is valid and email domain is 'g.ucla.edu'

// sets 'user_id' string reference to the subject claim found in token
// 'user_id' is unchanged if no subject claim is found
auth_level TokenAuth::get_auth(std::string id_token, std::string& user_id) {
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
      return NO_AUTH;
    }

   // check token issuer
    auto iss = decoded_token.get_issuer();
    std::string google1 = "accounts.google.com";
    std::string google2 = "https://accounts.google.com";
    if (iss != google1 && iss != google2) {
      // Google is not the issuer
      return NO_AUTH;
    }

    // get user id
    user_id = decoded_token.get_subject();

    // get email domain
    std::string email = decoded_token.get_payload_claim("email").as_string();
    std::string domain = email.substr(email.find_last_of("@") + 1);

    // return level of authorization
    if (domain == "g.ucla.edu") {
      return PRIVATE_AUTH;
    } else {
      return PUBLIC_AUTH;
    }
  } 
  catch (std::exception& e) {
    // verifier throws exeption if token signature is invalid
    return NO_AUTH;
  }
}
