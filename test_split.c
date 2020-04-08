#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include "fsplitter.h"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("USAGE: %s <in-file> <block-size>\n", basename(argv[0]));
		return 1;
	}
	int bs;
	if (1 != sscanf(argv[2], "%d", &bs))
	{
		printf("ERROR: invalid block size `%s`\n", argv[2]);
		return 1;
	}
	FILE *in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("ERROR: fopen(%s): %s\n", argv[1], strerror(errno));
		return 1;
	}
	SplitterPipe sp;
	char *name = malloc(strlen(argv[1]) + 10);
	sprintf(name, "%s.%%d", argv[1]);
	int res = SplitterPipeInit(&sp, name, bs);
	if (res != 0)
	{
		printf("ERROR: SplitterPipeInit: %s\n", strerror(res));
		return 1;
	}
	free(name);
	unsigned char buf[1024];
	while (1)
	{
		int cnt = fread(buf, 1, 1024, in);
		if (cnt > 0)
		{
			int res = fwrite(buf, 1, cnt, sp.Handle);
			if (res != cnt)
			{
				printf("ERROR: writing %d bytes, wrote %d\n", cnt, res);
				res = 1;
				break;
			}
		}
		if (cnt < 1024)
		{
			if (ferror(in))
			{
				perror("ferror");
				res = 1;
			}
			break;
		}
	}
	fclose(in);
	int rc = SplitterPipeWait(&sp);
	if (rc != 0) printf("ERROR: SplitterPipeWait: %s", strerror(rc));
}