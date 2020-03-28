#ifndef LZW_H
#define LZW_H

#include <stdio.h>

/*
On success, returns 0, otherwise:
1 - input file error
2 - output file error
3 - memeory allocation error
4 - input stream corrupted (for decompress only)
*/

int lzw_comp_file(const char *inFile, const char *outFile);
int lzw_compress(FILE *in, FILE *out);
int lzw_decomp_file(const char *inFile, const char *outFile);
int lzw_decompress(FILE *in, FILE *out);

#endif