#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "map.h"

int map_init(Map *m, int item_len, int key_len)
{
	if (item_len <= 0 || key_len <= 0 || key_len > item_len)
		return 1; //invalid argument
	m->item_len = item_len;
	m->key_len = key_len;
	m->cap = 0;
	m->cnt = 0;
	m->buf = NULL;
	return 0;
}

int map_setcap(Map *m, int cap)
{
	if (cap < m->cap)
		return 1;
	if (cap > m->cap)
	{
		void *buf = realloc(m->buf, cap * m->item_len);
		if (buf == NULL)
			return -1;
		m->cap = cap;
		m->buf = buf;
	}
	return 0;
}

int map_walk(Map *m, map_iter iter)
{
	int rc = 0;
	for (int i = 0; i < m->cnt; i++)
	{
		rc = iter(m->buf + i * m->item_len, m->item_len);
		if (rc != 0)
			break;
	}
	return rc;
}

void *map_find(Map *m, void *key, int *idx)
{
	int first = 0;
	int last = m->cnt - 1;
	int middle = (first + last) / 2;
	int rc;
	while (first <= last)
	{
		void *haystack = m->buf + m->key_len * middle;
		rc = memcmp(haystack, key, m->key_len);
		if (rc == 0)
			return haystack;
		if (rc < 0)
			first = middle + 1;
		else
			last = middle - 1;
		middle = (first + last) / 2;
	}
	if (idx != NULL)
		*idx = (last < 0) ? 0 : middle + 1;
	return NULL;
}

int map_add(Map *m, void *item)
{
	int idx;
	void *p = map_find(m, item, &idx);
	if (p != NULL)
	{
		memcpy(p, item, m->item_len);
		return 1;
	}
	if (m->cap == m->cnt)
	{
		void *buf = realloc(m->buf, (m->cap + 1) * m->item_len);
		if (buf == NULL)
			return -1;
		m->cap++;
		m->buf = buf;
	}
	m->cnt++;
	p = m->buf + m->item_len * idx;
	if (m->cnt > idx)
		memmove(p + m->item_len, p, (m->cnt - idx) * m->item_len);
	memcpy(p, item, m->item_len);
	return 0;
}

int map_del(Map *m, void *item)
{
	int idx;
	void *p = map_find(m, item, &idx);
	if (p == NULL)
		return 0;
	memmove(p, p + m->item_len, (m->cnt - idx) * m->item_len);
	m->cnt--;
	return 1;
}

int map_free(Map *m)
{
	free(m->buf);
	m->buf = NULL;
	m->cnt = 0;
	m->cap = 0;
}