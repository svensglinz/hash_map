#include "hash_map.h"
#include <string.h>

unsigned long default_hash(const void* key) {
  return (unsigned long) key;
}

int default_eq(const void* key1, const void* key2) {
  return key1 == key2;
}

void hash_map_init(hash_map** map, unsigned long (*hash_fn)(const void*), int (*cmp_fn)(const void*, const void*)) {
  size_t capacity = 16;

  *map = malloc(sizeof(hash_map));

  // initialize all values to 0
  hash_node** buckets_n = malloc(capacity * sizeof(hash_node*));

  if (buckets_n == NULL) {
    fprintf(stderr, "Failed to allocate memory for hash map\n");
    exit(1);
  }

  for (size_t i = 0; i < capacity; i++) {
    buckets_n[i] = NULL;
  }

  (*map)->buckets = buckets_n;
  (*map)->capacity = capacity;
  (*map)->hash_fn = hash_fn == NULL? default_hash: hash_fn;
  (*map)->cmp_fn = cmp_fn == NULL? default_eq: cmp_fn;
}

hash_node* hash_map_insert(hash_map* map, void* key, void* value) {

  if (hash_map_find(map, key)) {
    return NULL;
  }

  const unsigned long hash = map->hash_fn(key);
  const size_t index = hash % map->capacity;

  hash_node* node = malloc(sizeof(hash_node));
  if (node == NULL) {
    fprintf(stderr, "Error while allocating memory");
    exit(1);
  }

  node->key = key;
  node->value = value;
  node->next = NULL;

  if (map->buckets[index] == NULL) {
    map->buckets[index] = node;

  } else {
    hash_node* cur = map->buckets[index];
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = node;
  }
  return node;
}

hash_node* hash_map_find(const hash_map* map, const void* key) {
  const unsigned long hash = map->hash_fn(key);
  const size_t index = hash % map->capacity;

  hash_node* node = map->buckets[index];

  if (node == NULL) {
    return NULL;
  }
  while (node != NULL) {
    if (map->cmp_fn(node->key, key)) {
      return node;
    }
    node = node->next;
  }
  return NULL;
}

hash_map* hash_map_resize(hash_map* map) {
  if (map == NULL) {
    return NULL;
  }

  hash_node** buckets_n = realloc(map->buckets, map->capacity << 2 * sizeof(hash_node*));

  if (buckets_n == NULL) {
    fprintf(stderr, "Error while reallocating memory");
    exit(1);
  }
  map->buckets = buckets_n;
  return map;
}

void hash_map_clear(hash_map *map) {
  size_t idx = 0;
  hash_node* cur_bucket = map->buckets[0];

  while (cur_bucket != NULL) {
    hash_node* cur_elem = cur_bucket;
    while (cur_elem != NULL) {
      hash_node* next_elem = cur_elem->next;
      free(cur_elem);
      cur_elem = next_elem;
    }
    cur_bucket = map->buckets[++idx];
  }
}

void hash_map_free(hash_map* map) {
  hash_map_clear(map);
  free(map->buckets);
}

void hash_map_remove(hash_map *map, void *key) {
  const unsigned long hash = map->hash_fn(key);
  const size_t index = hash % map->capacity;

  hash_node* cur = map->buckets[index];
  hash_node* prev = NULL;
  while (cur != NULL) {
    if (map->cmp_fn(key, cur->key)) {
      if (prev != NULL) {
        prev->next = cur->next;
      } else {
        map->buckets[index] = cur->next;
      }
      free(cur);
      return;
    }
      prev = cur;
      cur = cur->next;
    }
}