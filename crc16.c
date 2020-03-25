#include "crc16.h"

uint16_t crc16(const uint8_t *data, int len)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;
    while (len--)
    {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}