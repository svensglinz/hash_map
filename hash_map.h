//
// Created by sven on 10/2/24.
//

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 16

typedef struct hash_node {
  void* key;
  void* value;
  struct hash_node* next;
} hash_node;

typedef struct hash_map {
  size_t size;
  size_t capacity;
  hash_node** buckets;
  unsigned long (*hash_fn)(const void*); // optional custom hash function
  int (*cmp_fn)(const void*, const void*); // optional custom comparator
} hash_map;

void hash_map_init(hash_map** map, unsigned long (*)(const void*), int (*)(const void*, const void*));
void hash_map_free(hash_map* map);
void hash_map_clear(hash_map* map);
hash_node* hash_map_insert(hash_map* map, void* key, void* value);
hash_node* hash_map_find(const hash_map* map, const void* key);
void hash_map_remove(hash_map* map, void* key);

#endif
