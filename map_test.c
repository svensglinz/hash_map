#include <stdio.h>
#include <stdlib.h>
#include "hash_map.h"

struct pair {
  int key;
  int value;
};

// custom hash function
static size_t hash_fn(const void* pair) {
  return ((struct pair*) pair)->key;
}

// custom comparator
int comp(const void* x, const void* y) {
  struct pair* p1 = (struct pair*) x;
  struct pair* p2 = (struct pair*) y;

  return p1->key == p2->key;
}

int main() {
  hash_map* map;
  hash_map_init(&map, sizeof(struct pair), hash_fn, comp, 10, 0.75f);

  printf("size: %lu\n", map->size);

  for (int i = 0; i < 5000000; i++) {
    hash_map_insert(map, &(struct pair){ .key= rand(), .value=68 });
  }

  hash_map_free(map);
  return 0;
}