#include <stdio.h>
#include <string.h>

#include "rc4.h"

void main()
{
    char *msg = "The quick brown fox jumps over a lazy dog.";
    char *key = "hello world";
    unsigned char data[100];
    memset(data, 0, 100);
    memcpy(data, msg, strlen(msg));
    RC4State rs;
    int len = strlen(msg);
    //Initialize RC4 status
    rc4InitState(&rs, key, strlen(key));
    rc4Crypt(&rs, data, len);
    for (int i = 0; i < len; i++)
    {
        printf("%02x ", data[i]);
    }
    printf("\n");
    //Re-do RC4 initialization for decrypt
    rc4InitState(&rs, key, strlen(key));
    rc4Crypt(&rs, data, len);
    printf("%s\n", data);
}