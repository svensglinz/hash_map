#include "hash_map.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// default hashing function
size_t default_hash(const void *key) {
    return (size_t) key;
}

// default comparator
int default_eq(const void *key1, const void *key2) {
    return key1 == key2;
}

hash_map *hash_map_resize(hash_map *map) {
    // double size
    size_t capacity_n = map->capacity << 1;

#ifdef DEBUG
  printf("rehashing to capacity: %lu\n", capacity_n);
#endif

    // temporarily increases memory to capacity old + capacity new
    hash_node **buckets_n = calloc(capacity_n, sizeof(hash_node *));

    if (buckets_n == NULL) {
        fprintf(stderr, "Error while reallocating memory");
        exit(1);
    }

    // rehash all keys
    hash_node **bucket_old = map->buckets;
    size_t capacity_old = map->capacity;

    map->buckets = buckets_n;
    map->capacity = capacity_n;

    for (size_t i = 0; i < capacity_old; i++) {
        hash_node *node = bucket_old[i];

        while (node != NULL) {
            hash_node *tmp = node->next;
            hash_map_insert_node(map, node);
            node = tmp;
        }
    }
    free(bucket_old);

#ifdef DEBUG
  printf("old load factor: %f\n", map->load_factor);
  printf("NAX load factor: %f\n", map->max_load_factor);
#endif

    map->load_factor = (float) map->size / (float) capacity_n;

#ifdef DEBUG
  printf("new load factor: %f\n", map->load_factor);
#endif

    return map;
}

void hash_map_init(hash_map **map, const size_t elem_size, unsigned long (*hash_fn)(const void *),
                   int (*cmp_fn)(const void *, const void *),
                   const size_t capacity, const float max_load_factor) {
    *map = malloc(sizeof(hash_map));

    // initialize all values to 0
    hash_node **buckets_n = calloc(capacity, sizeof(hash_node *));

    if (buckets_n == NULL) {
        fprintf(stderr, "Failed to allocate memory for hash map\n");
        exit(1);
    }

    // initialize fields
    (*map)->buckets = buckets_n;
    (*map)->max_load_factor = max_load_factor;
    (*map)->size = 0;
    (*map)->elem_size = elem_size;
    (*map)->load_factor = 0;
    (*map)->capacity = capacity;
    (*map)->hash_fn = hash_fn == NULL ? default_hash : hash_fn;
    (*map)->cmp_fn = cmp_fn == NULL ? default_eq : cmp_fn;
}

hash_node *allocate_node(const hash_map *map, const void *pair, const size_t hash) {
    hash_node *node = malloc(sizeof(hash_node) + map->elem_size);

    if (node == NULL) {
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }

    node->pair = (void *) (node + 1);
    node->hash = hash;
    node->next = NULL;

    if (node->pair == NULL) {
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }
    memcpy(node->pair, pair, map->elem_size);
    return node;
}

void *hash_map_insert(hash_map *map, const void *pair) {
    const size_t hash = map->hash_fn(pair);
    const size_t index = hash % map->capacity;
    hash_node *node;

    const hash_node *cur = map->buckets[index];
    if (cur == NULL) {
        node = allocate_node(map, pair, hash);
        map->buckets[index] = node;
    } else {
        while (cur != NULL) {
            if (map->cmp_fn(cur->pair, pair)) {
                return NULL;
            }
            cur = cur->next;
        }
        node = allocate_node(map, pair, hash);
        cur = node;
    }

    map->size++;
    map->load_factor = (float) (map->size) / (float) map->capacity;

    // resize of load factor is exceeded
    if (map->load_factor > map->max_load_factor) {
#ifdef DEBUG
    printf("detected load factor %f\n", map->load_factor);
#endif
        hash_map_resize(map);
    }
    return node;
}

void hash_map_insert_node(const hash_map *map, hash_node *node) {
    const size_t index = node->hash % map->capacity;
    hash_node *head = map->buckets[index];

    if (head == NULL) {
        map->buckets[index] = node;
        node->next = NULL; // will be terminal
    } else {
        map->buckets[index] = node;
        node->next = head;
    }
}

void *hash_map_find(const hash_map *map, const void *pair) {
    const unsigned long hash = map->hash_fn(pair);
    const size_t index = hash % map->capacity;

    const hash_node *node = map->buckets[index];

    if (node == NULL) {
        return NULL;
    }

    while (node != NULL) {
        if (map->cmp_fn(node->pair, pair)) {
            return node->pair;
        }
        node = node->next;
    }
    return NULL;
}

// sets entries in array to NULL and frees all elements in the linked list
// size of allocated array stays the same
void hash_map_clear(hash_map *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        hash_node *cur = map->buckets[i];

        while (cur != NULL) {
            hash_node *next = cur->next;
            free(cur);
            cur = next;
        }
    }
    map->size = 0;
}

void hash_map_free(hash_map *map) {
    hash_map_clear(map);
    free(map->buckets);
}

void hash_map_remove(hash_map *map, const void *key) {
    const unsigned long hash = map->hash_fn(key);
    const size_t index = hash % map->capacity;

    hash_node *cur = map->buckets[index];
    hash_node *prev = NULL;
    while (cur != NULL) {
        if (map->cmp_fn(key, cur->pair)) {
            if (prev != NULL) {
                prev->next = cur->next;
            } else {
                map->buckets[index] = cur->next;
            }
            free(cur);
            map->size--;
            map->load_factor = (float) (map->size) / (float) map->capacity;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
