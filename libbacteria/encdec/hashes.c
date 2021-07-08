#include "encdec/hashes.h"

void toSHA256(const uint8_t *data,
              char outputstring[SHA256_OUTPUTSTRING_SIZE]) {
  INITHASHFUNC(SHA256, 65);
}

void toSHA512(const uint8_t *data,
              char outputstring[SHA512_OUTPUTSTRING_SIZE]) {
  INITHASHFUNC(SHA512, 129);
}
