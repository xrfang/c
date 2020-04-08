#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "lzw.h"
#include "lzw_pipe.h"

static void *pipeSink(void *arg)
{
	LzwPipe *p = (LzwPipe *)arg;
	switch (p->op)
	{
	case 1:
		p->res = lzw_compress(p->input, p->output);
		break;
	case -1:
		p->res = lzw_decompress(p->input, p->output);
	}
	int res = fclose(p->input);
	if (p->res == 0)
		p->res = res;
}

static int lzwpInit(LzwPipe *zp, FILE *output)
{
	int fd[2];
	if (0 != pipe(fd))
		return errno;
	zp->input = fdopen(fd[0], "r");
	if (zp->input == NULL)
		return errno;
	zp->Handle = fdopen(fd[1], "w");
	if (zp->Handle == NULL)
		return errno;
	zp->output = output;
	return 0;
}

int LzwPipeInitCompressor(LzwPipe *zp, FILE *output)
{
	int rc = lzwpInit(zp, output);
	if (rc != 0)
		return rc;
	zp->op = 1;
	return pthread_create(&zp->tid, NULL, pipeSink, (void *)zp);
}

int LzwPipeInitDecompressor(LzwPipe *zp, FILE *output)
{
	int rc = lzwpInit(zp, output);
	if (rc != 0)
		return rc;
	zp->op = -1;
	return pthread_create(&zp->tid, NULL, pipeSink, (void *)zp);
}

int LzwPipeWait(LzwPipe *zp)
{
	int res = fclose(zp->Handle);
	pthread_join(zp->tid, NULL);
	return (zp->res == 0) ? res : zp->res;
}
