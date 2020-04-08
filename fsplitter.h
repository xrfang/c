#ifndef FSPLITTER_H
#define FSPLITTER_H

#include <pthread.h>
#include <stdio.h>

typedef struct SplitterPipe
{
	pthread_t tid;
	FILE *input;
	FILE *Handle;
	FILE *output;
	char *name;
	int bcnt;
	int bsize;
	int ccnt;
	int res;
} SplitterPipe;

int SplitterPipeInit(SplitterPipe *sp, const char *naming, int block);
int SplitterPipeWait(SplitterPipe *sp);

#endif