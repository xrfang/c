#ifndef LZW_PIPE_H
#define LZW_PIPE_H

#include <pthread.h>

typedef struct LzwPipe
{
	pthread_t tid;
	FILE *input;
	FILE *Handle;
	FILE *output;
	int res;
	int op;
} LzwPipe;

int LzwPipeInitCompressor(LzwPipe *zp, FILE *output);
int LzwPipeInitDecompressor(LzwPipe *zp, FILE *output);
int LzwPipeWait(LzwPipe *zp);

#endif
