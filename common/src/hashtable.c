/*
 * hashtable.c - Shriek's in-memory data store is implemented as a hashtable
 * with chaining for collision resolution. djb2 is the hash algorithm:
 * http://www.cse.yorku.ca/~oz/hash.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

/*
 * alloc_hashtable(): hashtable constructor
 */
hashtable_entry** alloc_hashtable(const size_t ht_size) {
  hashtable_entry** ht = calloc(ht_size, sizeof(hashtable_entry*));
  if (ht == NULL) {
    fprintf(stderr, "alloc_hashtable() | memory error\n");
    return NULL;
  }

  return ht;
}

/*
 * free_hashtable(): hashtable destructor
 */
void free_hashtable(hashtable_entry** ht, const size_t ht_size) {
  for (size_t i = 0; i < ht_size; i++) {
    if (ht[i] != NULL) {
      hashtable_entry* current = ht[i];
      hashtable_entry* next = NULL;
      while (current != NULL) {  // this throws a scan-build warning
        if (current->next != NULL) {
          next = current->next;
        }
        free_hashtable_entry(current);
        current = next;
      }
    }
  }
  free(ht);
}

/*
 * flush_hashtable(): serialize hashtable from memory to file
 */
ssize_t flush_hashtable(hashtable_entry* const* const ht,
                        const char* const path) {
  (void)ht;
  (void)path;
  return 0;
}

/*
 * restore_hashtable(): restore serialized hashtable from file to memory
 */
ssize_t restore_hashtable(hashtable_entry** const ht, const char* const path) {
  (void)ht;
  (void)path;
  return 0;
}

/*
 * alloc_hashtable_entry(): hashtable_entry constructor
 */
hashtable_entry* alloc_hashtable_entry(const char* const key,
                                       const char* const value) {
  hashtable_entry* hte = malloc(sizeof(hashtable_entry));
  if (hte == NULL) {
    fprintf(stderr, "alloc_hte() | Memory error.\n");
    return NULL;
  }

  hte->key = strdup(key);
  if (hte->key == NULL) {
    fprintf(stderr, "alloc_hte() | Memory error.\n");
    free(hte);
    return NULL;
  }

  hte->value = strdup(value);
  if (hte->value == NULL) {
    fprintf(stderr, "alloc_hte() | Memory error.\n");
    free(hte->key);
    free(hte);
    return NULL;
  }

  return hte;
}

/*
 * free_hashtable_entry(): hashtable_entry destructor
 */
void free_hashtable_entry(hashtable_entry* hte) {
  free(hte->key);
  free(hte->value);
  free(hte);
}

/*
 * hash_get(): look up a key in the hashtable and return the value
 */
char* hash_get(hashtable_entry** ht, const char* const key) {
  size_t hashval = djb2_hash(key);
  hashtable_entry* current = ht[hashval];
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return current->value;
    }
    current = current->next;
  }

  // Couldn't find it
  return NULL;
}

/*
 * hash_set(): install new k/v pair in the hashtable
 */
ssize_t hash_set(hashtable_entry** ht, const char* const key,
                 const char* const value) {
  // Check to see if the key is already in the table; if
  // so, dump the old value and install the new one.
  size_t hashval = djb2_hash(key);
  hashtable_entry* current = ht[hashval];
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      free(current->value);
      current->value = strdup(value);
      if (current->value == NULL) {
        fprintf(stderr, "hash_set() | Memory error.\n");
        return -1;
      } else {
        return 0;
      }
    }
    current = current->next;
  }

  // Otherwise, install the new entry as the first item
  hashtable_entry* hte = alloc_hashtable_entry(key, value);
  if (hte == NULL) {
    return -1;
  }
  hte->next = ht[hashval];
  ht[hashval] = hte;
  return 0;
}

/*
 * djb2_hash(): implements djb2 hashing.
 * source: http://www.cse.yorku.ca/~oz/hash.html
 */
size_t djb2_hash(const char* str) {
  size_t hash = 5381;
  unsigned char c;

  while ((c = (unsigned char)(*str++))) {
    /* hash * 33 + c */
    hash = ((hash << 5) + hash) + c;
  }

  return hash % DEFAULT_HT_SIZE;
}
