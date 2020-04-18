#ifndef MAP_H
#define MAP_H

#include <stddef.h>

typedef int (*map_iter)(const void *, size_t);
typedef int (*map_cmp)(const void *, const void *, size_t);

typedef struct
{
	size_t item_len; //length of each item
	size_t cnt;		 //item count
	size_t cap;		 //capacity of buf in term of # of items
	char *buf;
	map_cmp cmpfunc;
} Map;

void map_init(Map *m, size_t item_len);

//0=success; 1=cap too small; -1=out-of-memory
int map_setcap(Map *m, size_t cap);

//NULL=not found; otherwise pointer to item
//idx: if not NULL, store position of item found/to insert
void *map_find(const Map *m, void *key, size_t *idx);
void *map_find_addr(const Map *m, void *key, size_t *idx);

//0=inserted; 1=updated; -1=out-of-memory
int map_add(Map *m, void *item);
int map_add_addr(Map *m, void *item);

//0=not found; 1=deleted
int map_del(Map *m, void *item);
int map_del_addr(Map *m, void *item);

int map_walk(Map *m, map_iter iter);

void map_free(Map *m);

int map_cmpstr(const void *haystack, const void *needle, size_t len);

#endif