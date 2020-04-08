#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "lzw_pipe.h"

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("USAGE: %s <c|d> <in-file> <out-file>\n", basename(argv[0]));
		return 1;
	}
	FILE *in = fopen(argv[2], "r");
	if (in == NULL)
	{
		printf("ERROR: fopen(%s): %s\n", argv[2], strerror(errno));
		return 1;
	}
	FILE *out = fopen(argv[3], "w");
	if (out == NULL)
	{
		fclose(in);
		printf("ERROR: fopen(%s): %s\n", argv[3], strerror(errno));
		return 1;
	}
	LzwPipe zp;
	int res = 0;
	if (strcmp(argv[1], "c") == 0)
	{
		int res = LzwPipeInitCompressor(&zp, out);
		if (res != 0)
		{
			printf("LzwPipeInitCompressor: %s\n", strerror(res));
			return 1;
		}
	}
	else if (strcmp(argv[1], "d") == 0)
	{
		int res = LzwPipeInitDecompressor(&zp, out);
		if (res != 0)
		{
			printf("LzwPipeInitDecompressor: %s\n", strerror(res));
			return 1;
		}
	}
	else
	{
		printf("Invalid command `%s`, use `c` for compress and `d` for decompress\n", argv[1]);
		res = 1;
	}
	unsigned char buf[1024];
	while (1)
	{
		int cnt = fread(buf, 1, 1024, in);
		if (cnt > 0)
		{
			int res = fwrite(buf, 1, cnt, zp.Handle);
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
	int rc = fclose(in);
	int rp = LzwPipeWait(&zp);
	int ro = fclose(out);
	if (res != 0)
		return res;
	if (rc != 0)
	{
		printf("ERROR: closing input handle");
		return rc;
	}
	if (rp != 0)
	{
		printf("ERROR: LzwPipeWait");
		return rp;
	}
	if (ro != 0)
		printf("ERROR: closing output handle");
	return 0;
}