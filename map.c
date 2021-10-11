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
	Map *m = (Map *)malloc(sizeof(Map));
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
		char *buf = (char *)realloc(m->buf, cap * m->item_len);
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

int map_walk(Map *m, map_iter iter, void *user_ptr)
{
	int rc = 0;
	for (size_t i = 0; i < m->cnt; i++)
	{
		rc = iter(m->buf + i * m->item_len, m->item_len, user_ptr);
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

static int _map_add(Map *m, void *item, void *prev)
{
	size_t idx;
	char *p = (char *)map_find(m, item, &idx);
	if (p != NULL)
	{
		if (prev != NULL)
			memcpy(prev, p, m->item_len);
		memcpy(p, item, m->item_len);
		return 1;
	}
	if (m->cap == m->cnt)
	{
		char *buf = (char *)realloc(m->buf, (m->cap + 1) * m->item_len);
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

int map_add(Map *m, void *item)
{
	assert(item);
	return _map_add(m, item, NULL);
}

int map_add_addr(Map *m, void *item, void *prev)
{
	assert(item);
	return _map_add(m, &item, prev);
}

static int _map_del(Map *m, void *item, void *ptr)
{
	if (item == NULL)
		return 0;
	size_t idx;
	char *p = (char *)map_find(m, item, &idx);
	if (p == NULL)
		return 0;
	if (ptr != NULL)
		memcpy(ptr, p, m->item_len);
	memmove(p, p + m->item_len, (m->cnt - idx - 1) * m->item_len);
	m->cnt--;
	return 1;
}

int map_del(Map *m, void *item)
{
	return _map_del(m, item, NULL);
}

int map_del_addr(Map *m, void *item, void *ptr)
{
	if (item == NULL)
		return 0;
	return _map_del(m, &item, ptr);
}

size_t map_trim(Map *m, size_t new_cnt)
{
	size_t cnt = m->cnt;
	if (new_cnt < cnt)
		m->cnt = new_cnt;
	return cnt;
}

void *map_get(Map *m, size_t idx)
{
	if (idx >= m->cnt)
		return NULL;
	return m->buf + m->item_len * idx;
}

void *map_get_addr(Map *m, size_t idx)
{
	if (idx >= m->cnt)
		return NULL;
	return *(void **)(m->buf + m->item_len * idx);
}

void *map_rawbuf(Map *m)
{
	return m->buf;
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
	if (m->buf != NULL)
	{
		free(m->buf);
		m->buf = NULL;
	}
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