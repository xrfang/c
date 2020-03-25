#include <stdio.h>

#include "crc16.h"

void main()
{
    printf("%0X\n", crc16("123456789", 9));
}