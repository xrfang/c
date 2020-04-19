#ifndef MAP_H
#define MAP_H

#include <stddef.h>

typedef int (*map_iter)(const void *, size_t);
typedef int (*map_cmp)(const void *, const void *, size_t);
typedef struct Map Map;

Map *map_init(size_t item_len);
void map_free(Map *m);

size_t map_capacity(Map *m);
size_t map_count(Map *m);
void map_clear(Map *m);

//0=inserted; 1=updated; -1=out-of-memory
int map_add(Map *m, void *item);
int map_add_addr(Map *m, void *item);

//0=not found; 1=deleted
int map_del(Map *m, void *item);
int map_del_addr(Map *m, void *item);

//NULL=not found; otherwise pointer to item
//idx: if not NULL, store position of item found/to insert
void *map_find(const Map *m, void *key, size_t *idx);
void *map_find_addr(const Map *m, void *key, size_t *idx);

//0=success; 1=cap too small; -1=out-of-memory
int map_setcap(Map *m, size_t cap);
void map_setcmp(Map *m, map_cmp cmp);

int map_cmpstr(const void *haystack, const void *needle, size_t len);
int map_walk(Map *m, map_iter iter);

#endif