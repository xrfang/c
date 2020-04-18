#include <stdio.h>
#include <stdint.h>
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
	printf("has: %s\n", *(char **)buf);
	return 0;
}

int main(int argc, char **argv)
{
	Map m;
	map_init(&m, 1);
	map_setcap(&m, 10);
	printf("inserting to map...\n");
	char buf[7] = "kctepi";
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
	map_init(&m, sizeof(char *));
	m.cmpfunc = map_cmpstr;
	map_add_addr(&m, "a");
	map_add_addr(&m, "ab");
	map_add_addr(&m, "abcdefghi");
	map_add_addr(&m, "d");
	map_add_addr(&m, "de");
	map_add_addr(&m, "def");
	map_del_addr(&m, "de");
	map_del_addr(&m, "de"); //delete again has no effect
	map_add_addr(&m, "abc");
	size_t idx;
	char **p = map_find_addr(&m, "abc", &idx);
	if (p == NULL)
		printf("abc not found\n");
	else
		printf("found %s, idx=%ld\n", *p, idx);
	p = map_find_addr(&m, "abcd", NULL);
	if (p == NULL)
		printf("abcd not found\n");
	else
		printf("found: %s, idx=%ld\n", *p, idx);
	printf("map has %ld items\n", m.cnt);
	map_walk(&m, walkstr);
	map_free(&m);
}
