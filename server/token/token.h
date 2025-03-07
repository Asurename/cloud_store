#ifndef TOKEN_H
#define TOKEN_H

#include <l8w8jwt/decode.h>
#include <l8w8jwt/encode.h>
#include <string.h>

int encode(char *username,char **jwt);
int decode(char *JWT);

#endif