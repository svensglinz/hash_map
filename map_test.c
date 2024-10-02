#include <stdio.h>
#include <stdlib.h>
#include "hash_map.h"

// custom hash function
static unsigned long hash_fn(const void* key) {
  return ((unsigned long) key) * 100;
}

// custom comparator
int comp(const void* x, const void* y) {
  return (char*) x == (char*) y ? 1 : 0;
}

int main() {

  hash_map* map;
  hash_map_init(&map, &hash_fn, &comp);
  printf("%li\n", map->capacity);
  int a = 100;
  printf("%lul", map->hash_fn((void*) &a));
  hash_map_insert(map, (void*)"key1", (void*)"hello1");

  hash_map_insert(map, "key2", "hello2");
  hash_map_insert(map, "key3", "hello3");
  hash_map_insert(map, "key3", "hello4");

   char* arr[] = {"key1", "key2", "key3", "key4"};

   for (size_t i = 0; i < 4; i++) {
     hash_node* node = hash_map_find(map, arr[i]);
     if (node == NULL) {
       printf("key not found\n");
       } else {
         printf("Key: %s\n", (char*) node->key);
         printf("Value: %s\n", (char*) node->value);
       }
    }
    hash_map_remove(map, "key1");
    hash_node* node = hash_map_find(map, "key1");

     printf("%p\n", node);
}