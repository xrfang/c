#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

void find(Map *m, void *key)
{
	char *ptr = map_find(m, key, NULL);
	if (ptr == NULL)
		printf("%c not found\n", *(char *)key);
	else
		printf("found: %c\n", *ptr);
}

int show(const void *buf, size_t len, void *data)
{
	char c = ((char *)buf)[0];
	printf("%c", c);
	return 0;
}

int walk(const void *buf, size_t len, void *data)
{
	int *pos = (int *)data;
	*pos++;
	char c = ((char *)buf)[0];
	if (strchr("aeiou", c))
		return 1;
	return 0;
}

typedef struct
{
	int len;
	int pos;
	int idx;
} strec;

typedef struct
{
	char *key;
	char *val;
} strmap;

void strmap_add(Map *m, char *key, char *val)
{
	strmap *kv = malloc(sizeof(strmap));
	kv->key = malloc(strlen(key) + 1);
	strcpy(kv->key, key);
	kv->val = malloc(strlen(val) + 1);
	strcpy(kv->val, val);
	strmap *old;
	if (0 == map_add_addr(m, kv, &old))
		printf("[%s] is a new item\n", key);
	else
		printf("[%s] is an old item [%s => %s]\n", key, old->val, val);
}

void strmap_del(Map *m, char *key)
{
	strmap target = {.key = key};
	strmap *kv;
	if (0 == map_del_addr(m, &target, &kv))
		printf("[%s] is not in map\n", key);
	else
		printf("[%s] deleted from map [val=%s]\n", key, kv->val);
}

void strmap_clear(Map *m)
{
	strmap **kv = map_rawbuf(m);
	for (int i = 0; i < map_count(m); i++)
	{
		free(kv[i]->key);
		free(kv[i]->val);
		free(kv[i]);
	}
	map_clear(m);
}

int kvcmp(const void *haystack, const void *needle, size_t len)
{
	strmap *h = *(strmap **)haystack;
	strmap *n = *(strmap **)needle;
	return strcmp(h->key, n->key);
}

int walkstr(const void *buf, size_t len, void *data)
{
	strec *sr = (strec *)data;
	sr->idx++;
	char *str = *(char **)buf;
	if (strlen(str) > sr->len)
	{
		sr->len = strlen(str);
		sr->pos = sr->idx;
	}
	printf("has: %s\n", str);
	return 0;
}

int main(int argc, char **argv)
{
	Map *m = map_init(1);
	map_setcap(m, 10);
	printf("inserting to map...\n");
	char buf[7] = "kctepi";
	for (int i = 0; i < strlen(buf); i++)
		map_add(m, buf + i);
	printf("capacity: %ld; count: %ld\n", map_capacity(m), map_count(m));
	map_walk(m, show, NULL);
	printf("\n");
	printf("look for vowels in map...\n");
	int pos = -1;
	if (map_walk(m, walk, &pos))
		printf("found vowel letter at position %d\n", pos);
	else
		printf("vowel letter not found\n");
	printf("find items in map...\n");
	char *needle = "hithere";
	for (int i = 0; i < strlen(needle); i++)
		find(m, needle + i);
	printf("delete item from map...\n");
	for (int i = 0; i < strlen(needle); i++)
		if (map_del(m, needle + i))
			printf("%c deleted\n", *(char *)(needle + i));
		else
			printf("%c not found\n", *(char *)(needle + i));
	printf("look for vowels in map (again)...\n");
	pos = -1;
	if (map_walk(m, walk, &pos))
		printf("found vowel letter at position %d\n", pos);
	else
		printf("vowel letter not found\n");
	map_walk(m, show, NULL);
	printf("\n");
	printf("raw content of map:\n");
	char *raw = map_rawbuf(m);
	for (int i = 0; i < map_count(m); i++)
		printf("%c ", raw[i]);
	printf("\n");
	map_free(m);
	printf("now test string map...\n");
	m = map_init(sizeof(char *));
	map_setcmp(m, map_cmpstr);
	map_add_addr(m, "a", NULL);
	map_add_addr(m, "ab", NULL);
	map_add_addr(m, "abcdefghi", NULL);
	map_add_addr(m, "d", NULL);
	map_add_addr(m, "de", NULL);
	map_add_addr(m, "def", NULL);
	map_del_addr(m, "de", NULL);
	map_del_addr(m, "de", NULL); //delete again has no effect
	map_add_addr(m, "abc", NULL);
	size_t idx;
	char **p = map_find_addr(m, "abc", &idx);
	if (p == NULL)
		printf("abc not found\n");
	else
		printf("found %s, idx=%ld\n", *p, idx);
	p = map_find_addr(m, "abcd", NULL);
	if (p == NULL)
		printf("abcd not found\n");
	else
		printf("found: %s, idx=%ld\n", *p, idx);
	printf("map has %ld items\n", map_count(m));
	strec sr = {0, 0, -1};
	map_walk(m, walkstr, &sr);
	printf("longest string at position %d, length=%d\n", sr.pos, sr.len);
	char *lp = map_get_addr(m, sr.pos);
	printf("the longest string is: %s\n", lp);
	char *nx = map_get_addr(m, 100);
	assert(nx == NULL);
	map_clear(m);
	//map size of char* compatible with kv*
	map_setcmp(m, kvcmp);
	strmap_add(m, "a", "1");
	strmap_add(m, "a", "2");
	strmap_add(m, "b", "1");
	strmap_add(m, "b", "2");
	strmap_del(m, "b");
	strmap_del(m, "c");
	strmap_clear(m);
	strmap_clear(m); //test double-clear
	map_free(m);
}
