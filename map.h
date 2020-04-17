#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int (*map_iter)(const void *, size_t);
typedef int (*map_cmp)(const void *, const void *, size_t);

typedef struct
{
	int item_len; //length of each item
	int key_len;  //length of key (which is a prefix of item)
	int cnt;	  //item count
	int cap;	  //capacity of buf in term of # of items
	void *buf;
	map_cmp cmpfunc;
} Map;

//0=success; 1=invalid argument
int map_init(Map *m, int item_len, int key_len);

//0=success; 1=cap too small; -1=out-of-memory
int map_setcap(Map *m, int cap);

//NULL=not found; otherwise pointer to item
//idx: if not NULL, store position of item found/to insert
void *map_find(Map *m, void *key, int *idx);
void *map_find_addr(Map *m, void *key, int *idx);

//0=inserted; 1=updated; -1=out-of-memory
int map_add(Map *m, void *item);
int map_add_addr(Map *m, void *item);

//0=not found; 1=deleted
int map_del(Map *m, void *item);
int map_del_addr(Map *m, void *item);

int map_walk(Map *m, map_iter iter);

void map_free(Map *m);

#endif