#include <errno.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "packager.h"

static int mkdir_p(const char *path)
{
	char _path[PATH_MAX];
	char *p;

	if (strlen(path) > sizeof(_path) - 1)
		return ENAMETOOLONG;
	strcpy(_path, path); //Copy string so its mutable

	for (p = _path + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = '\0'; //Temporarily truncate
			if (mkdir(_path, S_IRWXU) != 0)
			{
				if (errno != EEXIST)
					return errno;
			}
			*p = '/';
		}
	}
	if (mkdir(_path, S_IRWXU) != 0)
	{
		if (errno != EEXIST)
			return errno;
	}
	return 0;
}

int PackagerInit(Packager *p, const char *name, int block, bool compress)
{
	p->compress = compress;
	char *path = malloc(strlen(name) + 1);
	strcpy(path, name);
	int rc = mkdir_p(dirname(path));
	free(path);
	if (rc != 0)
		return rc;
	rc = SplitterPipeInit(&p->sp, name, block);
	if (rc != 0)
		return rc;
	if (compress)
	{
		rc = LzwPipeInitCompressor(&p->zp, p->sp.Handle);
		if (rc != 0)
			return rc;
		p->Handle = p->zp.Handle;
	}
	else
		p->Handle = p->sp.Handle;
	p->Count = -1;
	return 0;
}

int PackagerWait(Packager *p)
{
	if (p->compress)
		LzwPipeWait(&p->zp);
	SplitterPipeWait(&p->sp);
	p->Count = p->sp.Count;
	if (p->compress && p->zp.res != 0)
		return p->zp.res;
	return p->sp.res;
}