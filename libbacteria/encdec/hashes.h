#include<openssl/sha.h>
#include<stdio.h>
#include<string.h>
#include"macros.h"

#define SHA256_OUTPUTSTRING_SIZE 65
#define SHA512_OUTPUTSTRING_SIZE 129
void toSHA256(const uint8_t * data, char outputstring[SHA256_OUTPUTSTRING_SIZE]);
void toSHA512(const uint8_t * data, char outputstring[SHA512_OUTPUTSTRING_SIZE]);
