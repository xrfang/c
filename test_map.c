#include <stdio.h>
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

void show(Map *m)
{
	for (int i = 0; i < m->cnt; i++)
		printf("%c", *(char *)(m->buf + i));
	printf("\n");
}

int walk(const void *buf, size_t len)
{
	char c = ((char *)buf)[0];
	if (strchr("aeiou", c))
		return 1;
	return 0;
}

int walkstr(const void *buf, size_t len)
{
	printf("has: %s\n", (char *)buf);
	return 0;
}

int comp(const void *haystack, const void *needle, size_t len)
{
	return strncmp((char *)haystack, (char *)needle, len);
}

int main(int argc, char **argv)
{
	Map m;
	map_init(&m, 1, 1);
	map_setcap(&m, 10);
	printf("inserting to map...\n");
	char buf[6] = "kctepi";
	for (int i = 0; i < strlen(buf); i++)
		map_add(&m, buf + i);
	show(&m);
	printf("look for vowels in map...\n");
	if (map_walk(&m, walk))
		printf("found vowel letter\n");
	else
		printf("vowel letter not found\n");
	printf("find items in map...\n");
	char *needle = "hithere";
	for (int i = 0; i < strlen(needle); i++)
		find(&m, needle + i);
	printf("delete item from map...\n");
	for (int i = 0; i < strlen(needle); i++)
		if (map_del(&m, needle + i))
			printf("%c deleted\n", *(char *)(needle + i));
		else
			printf("%c not found\n", *(char *)(needle + i));
	printf("look for vowels in map (again)...\n");
	if (map_walk(&m, walk))
		printf("found vowel letter\n");
	else
		printf("vowel letter not found\n");
	show(&m);
	map_free(&m);
	printf("now test string map...\n");
	map_init(&m, sizeof(char *), sizeof(char *));
	m.cmpfunc = comp;
	char a[10] = "a";
	char b[10] = "ab";
	char c[10] = "abc";
	char d[10] = "d";
	char e[10] = "de";
	char f[10] = "def";
	map_add(&m, a);
	map_add(&m, b);
	map_add(&m, c);
	map_add(&m, d);
	map_add(&m, e);
	map_add(&m, f);
	/*
	map_add(&m, "d");
	map_add(&m, "de");
	map_add(&m, "def");
	*/
	printf("map has %d items\n", m.cnt);
	map_walk(&m, walkstr);
	map_free(&m);
}
