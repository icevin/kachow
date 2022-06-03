#include "token_auth.hh"

#include <jwt-cpp/jwt.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <boost/log/trivial.hpp>
#include <string>
#include <fstream>
#include <sstream>

// returns the authorization level of a given 'id_token'
//   - NO_AUTH if token has invalid signature, incorrect or missing claims
//   - PUBLIC_AUTH if token is valid and email domain is NOT 'g.ucla.edu'
//   - PRIVATE_AUTH if token is valid and email domain is 'g.ucla.edu'

// sets 'user_id' string reference to the subject claim found in token
// 'user_id' is unchanged if no subject claim is found
auth_level TokenAuth::get_auth(std::string id_token, std::string& user_id) {
    try {
        std::ifstream in(public_key_path_, std::ios_base::binary);
        if (in.fail()) BOOST_LOG_TRIVIAL(fatal) << "Invalid public_key_path provided";

        std::stringstream in_buffer;
        in_buffer << in.rdbuf();

        std::string pem = in_buffer.str();

        // decode from base64
        auto decoded_token = jwt::decode(id_token);

        // parse JSON list of certificates, then retrieve proper cert using key id
        auto cert_list = jwt::details::map_of_claims<jwt::traits::kazuho_picojson>::parse_claims(pem);
        auto cert      = cert_list[decoded_token.get_key_id()];

        // extract key from certificate
        std::string key = jwt::helper::extract_pubkey_from_cert(jwt::traits::kazuho_picojson::as_string(cert));

        auto issuer = decoded_token.get_issuer();
        // precheck on issuer
        if (issuer.empty()) {
            return NO_AUTH;
        }

        // check token audience
        auto aud = decoded_token.get_audience();
        if (aud.find(client_id_) == aud.end()) {
            // client id not found in audience
            return NO_AUTH;
        }

        auto verifier =
            jwt::verify()
                .allow_algorithm(jwt::algorithm::rs256(key, "", "", ""))  // rsa 256 algo
                .with_issuer("https://accounts.google.com");               // enforce correct issuer

        verifier.verify(decoded_token);

        // get user id
        user_id = decoded_token.get_subject();

        // get email domain
        std::string email  = decoded_token.get_payload_claim("email").as_string();
        std::string domain = email.substr(email.find_last_of("@") + 1);

        // return level of authorization
        if (domain == "g.ucla.edu") {
            return PRIVATE_AUTH;
        } else {
            return PUBLIC_AUTH;
        }
    } catch (std::exception& e) {
        // verifier throws exeption if token signature is invalid
        BOOST_LOG_TRIVIAL(debug) << "Invalid token signature";
        return NO_AUTH;
    }
}

auth_level FakeAuth::get_auth(std::string id_token, std::string& user_id) {
    BOOST_LOG_TRIVIAL(debug) << "FAKE DECODING JWT";
    if (id_token == "PRIVAUTH1") {
        user_id = "priv1";
        return PRIVATE_AUTH;
    } else if (id_token == "PRIVAUTH2") {
        user_id = "priv2";
        return PRIVATE_AUTH;
    } else if (id_token == "PUBAUTH1") {
        user_id = "pub1";
        return PUBLIC_AUTH;
    }
    return NO_AUTH;
}