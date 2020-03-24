#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "lzw.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("USAGE: %s <c|d> <in-file> <out-file>\n", basename(argv[0]));
        return 1;
    }
    int res = 0;
    if (strcmp(argv[1], "c") == 0)
    {
        res = lzw_compress(argv[2], argv[3]);
        if (res != 0)
            perror("compress");
    }
    else if (strcmp(argv[1], "d") == 0)
    {
        res = lzw_decompress(argv[2], argv[3]);
        if (res != 0)
            perror("decompress");
    }
    else
    {
        printf("Invalid command `%s`, use `c` for compress and `d` for decompress\n", argv[1]);
        res = 1;
    }
    return res;
}
