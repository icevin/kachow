#include "token_auth.hh"

#include <jwt-cpp/jwt.h>

#include <string>

#include "gtest/gtest.h"

class TokenAuthTest : public ::testing::Test {
  protected:
    std::string s;
    TokenAuth* t;

    void SetUp() override {
        s = "init";
        t = new TokenAuth("test123.apps.googleusercontent.com", "./rsa_custom_pem", "TEST_ISSUER");
    }

    void TearDown() override {
        delete t;
    }
};

TEST_F(TokenAuthTest, TokenAuthPublic) {
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJURVNUX0lTU1VFUiIsInN1YiI6IkZBS0VfSUQiLCJhdWQiOiJ0ZXN0MTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiaWF0IjoxNjAwMDAwMDAwLCJlbWFpbCI6ImNzMTMwQGdtYWlsLmNvbSJ9.JYEuJONremIuA3x8ogKDY8rgeZRtNYXpM5UGQ0w4gePUfr_-JmncZBqeq1kaQO3QrEa3ZVRxUpZRZlsEwxZaDGibxQ0p74vxFXipXK2ci0TNV3y9SEun-eVs-pQQFL0cEdBqYNSYSSgEe3wIOLrJ_w9KjNkI9ZSy_2KevGaoPLoUSy041UPp4nsqfO8saPV71_YE8vRTUXfPvxNwqMhCbm_9H_2BTFtyRm27hd2FUo9XBKz7FzWsnNppx2lW-i_Nj8_G5HsRyJMMpESmXGxxOgnBzd4JLKLPeDwqtd58xU6CLWtiDI-jDC-gHc6di2wkcegWxdjiuAURODqfyZpXzA";

    // "iss": "TEST_ISSUER",
    // "sub": "FAKE_ID",
    // "aud": "test123.apps.googleusercontent.com",
    // "kid": "1",
    // "iat": 1600000000,
    // "exp": 9999999999,
    // "email": "cs130@gmail.com"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, PUBLIC_AUTH);
    EXPECT_EQ(s, "FAKE_ID");
}

TEST_F(TokenAuthTest, TokenAuthExpired) {
    std::string token = "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJURVNUX0lTU1VFUiIsInN1YiI6IkZBS0VfSUQiLCJhdWQiOiJ0ZXN0MTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiaWF0IjoxNjAwMDAwMDAwLCJleHAiOjE2NTQzMDQ3MTcsImVtYWlsIjoiY3MxMzBAZy51Y2xhLmVkdSJ9.Wbieo06NXGX40uSKtg3HSAWJK3WAmgtp30gdE3uIztE64r4wyeUhYyCJkKaAaC-WoY2_U6wodPYYELyg5mcqCZo_eKMrQv87AlpWyZhQnKQcxQ7DrDXeZugmqz7vLwCMshJFaqPbTb--Yn2JuKts-IMQm2ACaKDU7O-pLgh6nkrbuAHJr0iyZrsOqXM8xN8Hio_egSFH_Tr22Ig6cMKT9n16Xfhd1UpXFHRWeSgATyY8ULSl7si0SsH-WpsoN2t-LQgVwRipOOuS9WpumIpbUNVCeV8T17_30TERQxhymc04JjsrPN08j3c1EIVAGh2ev9T93PuZVHS5a4QzdyAukg";

    // "iss": "TEST_ISSUER",
    // "sub": "FAKE_ID",
    // "aud": "test123.apps.googleusercontent.com",
    // "kid": "1",
    // "iat": 1600000000,
    // "exp": 1600000001,
    // "email": "cs130@g.ucla.edu"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, NO_AUTH);
}

TEST_F(TokenAuthTest, TokenAuthPrivate) {
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJURVNUX0lTU1VFUiIsImF1ZCI6InRlc3QxMjMuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJzdWIiOiJGQUtFX0lEIiwia2lkIjoiMSIsImlhdCI6MTYwMDAwMDAwMCwiZW1haWwiOiJjczEzMEBnLnVjbGEuZWR1In0.LPWV3bJe98c1YZ3BJhs8tv034u-3Hu-JhcFhpgLkWesrCMS2Jl715QqphJqtSavKHZJnWOE1K_l1N1LSO0mZVmxzQVflkHPKl5NirogI5N1Rcpffsa0tBfBpqArv9_5UfBpPQV_cAdq3iSkDCCqzeHhyLYYjEAqujINRVeysHz65RMM_2zL5xVh_Yo3OeTfbiTP4AEklfcczeeOYbyqw6Tny9_--TRunuvL_abLviGR-vBUTwaUDPduo78p12RXyI0cUxnZ4xJ9KdtX8DxMV7ldkPJ6OZbOaxKJ79mDJUgBgAcUMuGWyaR1gFFVPmxsj4kcPDN3kiFGuUvj8tkgneQ";

  // "iss": "TEST_ISSUER",
  // "aud": "test123.apps.googleusercontent.com",
  // "sub": "FAKE_ID",
  // "kid": "1",
  // "iat": 1600000000,
  // "email": "cs130@g.ucla.edu"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, PRIVATE_AUTH);
}

TEST_F(TokenAuthTest, InvalidIssuer) {
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJCQURfSVNTVUVSIiwic3ViIjoiRkFLRV9JRCIsImF1ZCI6InRlc3QxMjMuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJpYXQiOjE2MDAwMDAwMDAsImVtYWlsIjoiY3MxMzBAZy51Y2xhLmVkdSJ9.TQEhVwyWfJN2Uk4vXEY9wYpHtSNH0JRuLBS8hjBH2PNIepF0F47_o_MS-J5_NcgaS7IB1RZgElzO3etlDS46oEtTewCc9wJ_wDcjIs84XOy2RP6bXLra9ERIxzBijZiUSNvXQZdf_R3BU8cKHlini5JatgYwCOMN9dVkU3NbphnkCDewPjG6Z8RxukbKutFv2q7YWX_ft4-RwGClc1wSgSNFPl-bbH1TRfB8jYOlZkv4ZxjlpAdabXoxfppdBiLg1PwhmA71Nz0Bh2yF23gMDDahC-aRQNOqG1ZFpoKTuH4rrm86QmyB9B_xj3R_U4K-HFwdQjg2d9D4lpj9UvTVaw";

    // "iss": "BAD_ISSUER",
    // "sub": "FAKE_ID",
    // "aud": "test123.apps.googleusercontent.com",
    // "iat": 1600000000,
    // "email": "cs130@g.ucla.edu"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, NO_AUTH);
    EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidAudience) { 
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJURVNUX0lTU1VFUiIsInN1YiI6IkZBS0VfSUQiLCJhdWQiOiJ6enp6dGVzdDEyMy5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlhdCI6MTYwMDAwMDAwMCwiZW1haWwiOiJjczEzMEBnLnVjbGEuZWR1In0.cF4wOVgfJRSNCPpOm8hn3vrE4xkoC1UfS-CaIauPX5UmK-G-OA5UHHUOaRPua9bI_T5zGyesiskac7iiRHILKD0mWC72Wn_WKzJsgo3DxT4rWJbYJcc7Zn3vrR1T-CIYyauOeneDcmh1yo31pz7McsQTX1JOEp8dDqdEyEKoYkDsJCmlZNkJZcWZOQ9DT-RAeHR9znz44YkVH41lYA2imoubTUEHRkGDUBUqIXGNI83XhLkE3BxJBTBa0H8XLHEgZ-b9SDSMc6va72KVcmgdAor4sdqFwwapIDgGT1YNn3bjdPK1a9Sq8Z1UvGi3PzzE9iUFb3O7Lqdl_6aZUgq4qw";

    // "iss": "TEST_ISSUER",
    // "sub": "FAKE_ID",
    // "kid": "1",
    // "iat": 1600000000,
    // "email": "cs130@g.ucla.edu"
    // "aud": "test123.apps.googleusercontent.com"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, NO_AUTH);
    EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, InvalidSignature) {
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJCQURfSVNTVUVSIiwic3ViIjoiRkFLRV9JRCIsImF1ZCI6Inp6enp0ZXN0MTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiaWF0IjoxNjAwMDAwMDAwLCJleHAiOjk5OTk5OTk5OTksImVtYWlsIjoiY3MxMzBAZy51Y2xhLmVkdSJ9.PvlHxvU0LGw3fWPebRl7CfJAb9udXiFTlU94K3oI0gkNBwgGYPvc3qn5xI7FsNL_5r5btCVp1nUb_35wYFnHylxJkHcql5SrsmL3S_wvmJXCYBQ-s3G8d_dih2cU3zJ8fCyGSNe8chw7ZLYZdDYrMk7z6MvCg9qHdk1-28-OWiG14x25Mb6AQFYwCmu8vTzTpJdUP_xbFMr2fa_eSfGAKg8A53mmKYplCMkSsUMvEHkdGjg47w-mz1r2bNdahpCKWfYWzJSqyipwz0OauQMFqyRStyKSL2Em-ik94FNIY-Iv2dl4BLQvTNMKoGf0mHOslU3rXDuEMDIthNRWZgResA";

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, NO_AUTH);
    EXPECT_EQ(s, "init");
}

TEST_F(TokenAuthTest, NoSubject) {
    std::string token =
        "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJCQURfSVNTVUVSIiwiYXVkIjoidGVzdDEyMy5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlhdCI6MTYwMDAwMDAwMCwiZW1haWwiOiJjczEzMEBnLnVjbGEuZWR1In0.cqMv1kYAC8T-W_H7KeKjf-Td7KY5QhY4-0nj9dcCYerJp45ipcFd6yNMUA-exYsrIn943G1K2ENE34endqoKDFnei9UD3opxS0SuvtxygcDuk_pjjV-JnK2DWOtvBUDTLcIQXGQo7pVutszbxSL-600Sbrf4C5WfXVtwyyHJv4QJ00v0nT0efO269fd3ZlQx65iUw3GW4DFAHKBvkkZXLJJXXWLGw3JeQFGl3YovYJwpi0RV9AM1u0DfAJgG5MduY4hyakL1xjO4Cfxqh2faiJNYOWE2b85b9hbnB39LQ_0qVToVO7FZl_845PkhbcjnuvLMfsnoiYLn6rhuD3uOdw";

    // "iss": "BAD_ISSUER",
    // "aud": "test123.apps.googleusercontent.com",
    // "iat": 1600000000,
    // "email": "cs130@g.ucla.edu"

    auth_level lvl = t->get_auth(token, s);
    EXPECT_EQ(lvl, NO_AUTH);
    EXPECT_EQ(s, "init");
}
