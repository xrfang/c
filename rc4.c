/* RC4 cipher
 * https://codereview.stackexchange.com/questions/41148
 */
#include "rc4.h"

static void swap(uint8_t *a, uint8_t *b)
{
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

void rc4InitState(RC4State *state, const uint8_t K[256], const uint8_t keylen)
{
    uint8_t T[256];
    for (int i = 0; i < 256; i++)
    {
        state->S[i] = i;
        T[i] = K[i % keylen];
    }
    //Initial permutation of S
    uint8_t *S = state->S;
    int j = 0;
    for (int i = 0; i < 256; i++)
    {
        j = (j + S[i] + T[i]) % 256;
        swap(&S[i], &S[j]);
    }
    //Initialize counters in state
    state->i = state->j = 0;
}

static uint8_t rc4CryptByte(RC4State *state, uint8_t text)
{
    uint8_t *S = state->S;
    uint8_t i = ++(state->i);
    uint8_t j = (state->j += S[i]);
    swap(&S[i], &S[j]);
    uint8_t t = S[i] + S[j];
    uint8_t k = S[t];
    return text ^ k;
}

void rc4Crypt(RC4State *state, uint8_t text[], uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        text[i] = rc4CryptByte(state, text[i]);
}