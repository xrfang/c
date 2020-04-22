#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fsplitter.h"

static int openOutput(SplitterPipe *sp)
{
	if (sp->output != NULL)
	{
		int res = fclose(sp->output);
		if (res != 0)
			return errno;
		sp->ccnt = 0;
	}
	sp->bcnt++;
	char *fn = malloc(strlen(sp->name) + 10);
	sprintf(fn, sp->name, sp->bcnt);
	sp->output = fopen(fn, "w");
	free(fn);
	return (sp->output == NULL) ? errno : 0;
}

static void *pipeSink(void *arg)
{
	SplitterPipe *p = (SplitterPipe *)arg;
	p->res = openOutput(p);
	if (p->res != 0)
		return NULL;
	while (1)
	{
		int c = fgetc(p->input);
		if (c == EOF)
			break;
		fputc(c, p->output);
		p->ccnt++;
		if (p->ccnt >= p->bsize)
		{
			p->res = openOutput(p);
			if (p->res != 0)
				return NULL;
		}
	}
	fclose(p->output);
	return NULL;
}

int SplitterPipeInit(SplitterPipe *sp, const char *naming, int block)
{
	int fd[2];
	if (0 != pipe(fd))
		return errno;
	sp->input = fdopen(fd[0], "r");
	if (sp->input == NULL)
		return errno;
	sp->Handle = fdopen(fd[1], "w");
	if (sp->Handle == NULL)
		return errno;
	sp->output = NULL;
	sp->bsize = block;
	sp->bcnt = 0;
	sp->ccnt = 0;
	sp->name = malloc(strlen(naming) + 4);
	strcpy(sp->name, naming);
	sp->Count = -1;
	if (strstr(sp->name, "%d") == NULL)
		strcat(sp->name, "%d");
	return pthread_create(&sp->tid, NULL, pipeSink, (void *)sp);
}

int SplitterPipeWait(SplitterPipe *sp)
{
	int res = fclose(sp->Handle);
	if (res != 0)
		res = errno;
	pthread_join(sp->tid, NULL);
	sp->Count = sp->bcnt;
	if (sp->res == 0)
		sp->res = res;
	free(sp->name);
	return sp->res;
}