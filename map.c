#include "map.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct Map
{
	size_t item_len; //length of each item
	size_t cnt;		 //item count
	size_t cap;		 //capacity of buf in term of # of items
	char *buf;
	map_cmp cmpfunc;
};

Map *map_init(size_t item_len)
{
	assert(item_len > 0);
	Map *m = malloc(sizeof(Map));
	m->item_len = item_len;
	m->cap = 0;
	m->cnt = 0;
	m->buf = NULL;
	m->cmpfunc = NULL;
	return m;
}

int map_setcap(Map *m, size_t cap)
{
	if (cap < m->cnt)
		return 1;
	if (cap != m->cap)
	{
		void *buf = realloc(m->buf, cap * m->item_len);
		if (buf == NULL)
			return -1;
		m->cap = cap;
		m->buf = buf;
	}
	return 0;
}

void map_setcmp(Map *m, map_cmp cmp)
{
	m->cmpfunc = cmp;
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

void *map_find(const Map *m, void *key, size_t *idx)
{
	if (key == NULL)
		return NULL;
	map_cmp cmpfunc = (m->cmpfunc == NULL) ? memcmp : m->cmpfunc;
	int first = 0;
	int last = m->cnt - 1;
	int middle = first + (last - first) / 2;
	void *ptr = NULL;
	while (first <= last)
	{
		void *haystack = m->buf + m->item_len * middle;
		int rc = cmpfunc(haystack, key, m->item_len);
		if (rc == 0)
		{
			ptr = haystack;
			break;
		}
		if (rc < 0)
			first = middle + 1;
		else
			last = middle - 1;
		middle = (first + last) / 2;
	}
	if (idx != NULL)
		*idx = (ptr == NULL) ? ((last < 0) ? 0 : middle + 1) : middle;
	return ptr;
}

void *map_find_addr(const Map *m, void *key, size_t *idx)
{
	if (key == NULL)
		return NULL;
	return map_find(m, &key, idx);
}

int map_add(Map *m, void *item)
{
	assert(item);
	size_t idx;
	char *p = map_find(m, item, &idx);
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
	p = m->buf + m->item_len * idx;
	if (m->cnt > idx)
		memmove(p + m->item_len, p, (m->cnt - idx) * m->item_len);
	memcpy(p, item, m->item_len);
	m->cnt++;
	return 0;
}

int map_add_addr(Map *m, void *item)
{
	assert(item);
	return map_add(m, &item);
}

int map_del(Map *m, void *item)
{
	if (item == NULL)
		return 0;
	size_t idx;
	char *p = map_find(m, item, &idx);
	if (p == NULL)
		return 0;
	memmove(p, p + m->item_len, (m->cnt - idx - 1) * m->item_len);
	m->cnt--;
	return 1;
}

int map_del_addr(Map *m, void *item)
{
	if (item == NULL)
		return 0;
	return map_del(m, &item);
}

size_t map_count(Map *m)
{
	return m->cnt;
}

size_t map_capacity(Map *m)
{
	return m->cap;
}

void map_clear(Map *m)
{
	free(m->buf);
	m->buf = NULL;
	m->cnt = 0;
	m->cap = 0;
}

void map_free(Map *m)
{
	map_clear(m);
	free(m);
}

int map_cmpstr(const void *haystack, const void *needle, size_t len)
{
	char *ptr1 = *(char **)haystack;
	char *ptr2 = *(char **)needle;
	return strcmp(ptr1, ptr2);
}