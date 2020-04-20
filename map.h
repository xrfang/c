/*
map -- simple generic map structure
SPDX-License-Identifier: MIT
Copyright (C) 2020, Xiangrong Fang
Homepage: https://github.com/xrfang/c
*/
#ifndef MAP_H
#define MAP_H

#include <stddef.h>

//iterator, used by map_walk()
//arguments: pointer to current item, item_length and
//user pointer.
//return non-zero will cause map_walk() to abort iteration
typedef int (*map_iter)(const void *, size_t, void *);

//comparator, used by map_find() and map_find_addr()
//signature same as memcmp()
typedef int (*map_cmp)(const void *, const void *, size_t);

//the Map structure is defined in map.c
typedef struct Map Map;

//initialize a map structure with item_len.  Internally,
//Map is implemented as a dynamic array, with elements of
//same length.
Map *map_init(size_t item_len);

//clear map buffer, reseet count and capacity to 0
//map is still usable after clear.
void map_clear(Map *m);
//clear map buffer and release the map itself
void map_free(Map *m);

//return capacity of map buffer (in term of item count)
//for memory occupation, time the value by item_length
size_t map_capacity(Map *m);
//return number of items in the map
size_t map_count(Map *m);

//add item into the map. return 0 if the item was not
//in the map; 1 if the item was there and updated; -1
//for out-of-memory.
int map_add(Map *m, void *item);
//add address of item into the map.
int map_add_addr(Map *m, void *item);

//delete item from map. return 0 if item was not found;
//1 if item was found and deleted.
int map_del(Map *m, void *item);
//delete address of item from map.
int map_del_addr(Map *m, void *item);

//search for item in the map. if found, return pointer
//to the item, otherwise NULL. the second argument idx
//is optional. if not NULL, returns the position of 
//item found or to be inserted.  position is 0-based.
void *map_find(const Map *m, void *key, size_t *idx);
//search for address of item in the map.
void *map_find_addr(const Map *m, void *key, size_t *idx);

//get item at specified offset, if idx out of range, return NULL
void *map_get(Map *m, size_t idx);
//get item address at specified offset.
void *map_get_addr(Map *m, size_t idx);

//get the raw buffer of the map.
void *map_rawbuf(Map *m);

//set map capacity. return 0 if successful; 1 means
//new capacity is too small for the items currently
//in map; -1 means out of memory.
int map_setcap(Map *m, size_t cap);

//set comparator for the map. by default, map uses
//memcmp as comparator.
void map_setcmp(Map *m, map_cmp cmp);
//string comparator provided for convenience.
int map_cmpstr(const void *haystack, const void *needle, size_t len);

//emit each item in the map to the iterator. return 0
//if all items are traversed.  if the iterator returns
//non-zero, the process is aborted, and that value is
//returned.
int map_walk(Map *m, map_iter iter, void *user_ptr);

#endif