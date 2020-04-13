#ifndef AIRSWITCH_PACKGER_H
#define AIRSWITCH_PACKGER_H

#include <stdbool.h>

#include "fsplitter.h"
#include "lzw_pipe.h"

typedef struct Packager
{
	FILE *Handle;
	LzwPipe zp;
	SplitterPipe sp;
	bool compress;
	int Count;
} Packager;

int PackagerInit(Packager *p, const char *name, int block, bool compress);
int PackagerWait(Packager *p);

#endif