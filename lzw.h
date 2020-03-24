#ifndef LZW_H
#define LZW_H

/*
On success, returns 0, otherwise:
1 - input file error
2 - output file error
3 - memeory allocation error
4 - input stream corrupted (for decompress only)
*/

int lzw_compress(const char *inFile, const char *outFile);
int lzw_decompress(const char *inFile, const char *outFile);

#endif