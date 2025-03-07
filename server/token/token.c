#include "token.h"

static const char KEY[] = "a-very-long-secret-key-with-at-least-64-characters-1234567890abcdefghijklmnopqrstuvwxyz";
// static const char JWT[] = "eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCIsImtpZCI6InNvbWUta2V5LWlkLWhlcmUtMDEyMzQ1In0.eyJpYXQiOjE1ODAzMzk1OTQsImV4cCI6MTU4MDM0MDE5NCwic3ViIjoiR29yZG9uIEZyZWVtYW4iLCJpc3MiOiJCbGFjayBNZXNhIiwiYXVkIjoiQWRtaW5pc3RyYXRvciIsImN0eCI6IlVuZm9yc2VlbiBDb25zZXF1ZW5jZXMiLCJhZ2UiOjI3LCJzaXplIjoxLjg1LCJhbGl2ZSI6dHJ1ZSwibnVsbHRlc3QiOm51bGx9.1wUyFChHp7CwSVJ7ZHYv0mMphSwaCZQyn-LMDLRBskxikyhqvLNehQNz3T--FJ3OJ3LxL6FrI6KFuTVGqcIV4g";


int encode(char *username,char **pjwt)
{
    char* jwt;
    //size_t jwt_length;

    size_t jwt_length = 4096;

    struct l8w8jwt_claim header_claims[] =
    {
        {
            .key = "username",
            .key_length = strlen("username"),
            .value = username,
            .value_length = strlen(username),
            .type = L8W8JWT_CLAIM_TYPE_STRING
        }
    };

    // struct l8w8jwt_claim payload_claims[] =
    // {
    //     {
    //         .key = "ctx",
    //         .key_length = 3,
    //         .value = "Unforseen Consequences",
    //         .value_length = strlen("Unforseen Consequences"),
    //         .type = L8W8JWT_CLAIM_TYPE_STRING
    //     },
    //     {
    //         .key = "age",
    //         .key_length = 3,
    //         .value = "27",
    //         .value_length = strlen("27"),
    //         .type = L8W8JWT_CLAIM_TYPE_INTEGER
    //     },
    //     {
    //         .key = "size",
    //         .key_length = strlen("size"),
    //         .value = "1.85",
    //         .value_length = strlen("1.85"),
    //         .type = L8W8JWT_CLAIM_TYPE_NUMBER
    //     },
    //     {
    //         .key = "alive",
    //         .key_length = strlen("alive"),
    //         .value = "true",
    //         .value_length = strlen("true"),
    //         .type = L8W8JWT_CLAIM_TYPE_BOOLEAN
    //     },
    //     {
    //         .key = "nulltest",
    //         .key_length = strlen("nulltest"),
    //         .value = "null",
    //         .value_length = strlen("null"),
    //         .type = L8W8JWT_CLAIM_TYPE_NULL
    //     }
    // };

    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;

    params.sub = "transfile";
    params.sub_length = strlen("transfile");

    params.iss = "ke";
    params.iss_length = strlen("ke");

    params.aud = "cloud-storage user";
    params.aud_length = strlen("cloud-storage user");

    params.iat = l8w8jwt_time(NULL);
    params.exp = l8w8jwt_time(NULL) + 5; // Set to expire after 10 minutes (600 seconds).

    params.additional_header_claims = header_claims;
    params.additional_header_claims_count = sizeof(header_claims) / sizeof(struct l8w8jwt_claim);

    // params.additional_payload_claims = payload_claims;
    // params.additional_payload_claims_count = sizeof(payload_claims) / sizeof(struct l8w8jwt_claim);

    params.secret_key = (unsigned char*)KEY;
    params.secret_key_length = strlen(KEY);

    params.out = &jwt;
    params.out_length = &jwt_length;

    int r = l8w8jwt_encode(&params);
    printf("\nl8w8jwt_encode_hs512 function returned %s (code %d).\n\nCreated token: \n%s\n", r == L8W8JWT_SUCCESS ? "successfully" : "", r, jwt);
    strcpy(pjwt,jwt);
    //(*jwt)[jwt_length] = '\0';
    //l8w8jwt_free(jwt);
    return 0;
}

int decode(char *JWT)
{
    struct l8w8jwt_decoding_params params;
    l8w8jwt_decoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;

    params.jwt = (char*)JWT;
    params.jwt_length = strlen(JWT);

    params.verification_key = (unsigned char*)KEY;
    params.verification_key_length = strlen(KEY);

    params.validate_iss = "ke";
    params.validate_iss_length = strlen(params.validate_iss);

    params.validate_sub = "transfile";
    params.validate_sub_length = strlen(params.validate_sub);

    params.validate_exp = 1;
    params.exp_tolerance_seconds = 0;

    params.validate_iat = 1;
    params.iat_tolerance_seconds = 60;

    enum l8w8jwt_validation_result validation_result;
    int r = l8w8jwt_decode(&params, &validation_result, NULL, NULL);

    printf("\nl8w8jwt_decode_hs512 function returned %s (code %d).\n\nValidation result: \n%d\n", r == L8W8JWT_SUCCESS ? "successfully" : "", r, validation_result);

    return validation_result;
}

