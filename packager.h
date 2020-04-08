#ifndef AIRSWITCH_PACKGER_H
#define AIRSWITCH_PACKGER_H

#include "fsplitter.h"
#include "lzw_pipe.h"

typedef struct Packager
{
	FILE *Handle;
	LzwPipe zp;
	SplitterPipe sp;
} Packager;

int PackagerInit(Packager *p, const char *name, int block);
int PackagerWait(Packager *p);

#endif