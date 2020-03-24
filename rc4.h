#ifndef RC4_H
#define RC4_H

#include <stdint.h>

typedef struct
{
    uint8_t i, j;
    uint8_t S[256];
} RC4State;

void rc4InitState(RC4State *state, const uint8_t K[256], const uint8_t keylen);
void rc4Crypt(RC4State *state, uint8_t text[], uint32_t len);

#endif