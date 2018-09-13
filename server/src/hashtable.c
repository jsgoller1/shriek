#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

/*
 * hashtable.c - Shriek's in-memory data store is implemented as a hashtable
 * with chaining for collision resolution. djb2 is the hash algorithm:
 * http://www.cse.yorku.ca/~oz/hash.html
 */

hashtable_entry* hashtable[DEFAULT_HT_SIZE] = {NULL};

/*
 * create_hashtable_entry(): hashtable_entry constructor
 */
hashtable_entry* create_hashtable_entry(const char* const key,
                                        const char* const value) {
  hashtable_entry* hte = malloc(sizeof(hashtable_entry));
  if (hte == NULL) {
    fprintf(stderr, "create_hashtable_entry() | Memory error.\n");
    return NULL;
  }

  hte->key = strdup(key);
  if (hte->key == NULL) {
    fprintf(stderr, "create_hashtable_entry() | Memory error.\n");
    free(hte);
    return NULL;
  }

  hte->value = strdup(value);
  if (hte->value == NULL) {
    fprintf(stderr, "create_hashtable_entry() | Memory error.\n");
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
char* hash_get(const char* const key) {
  size_t hashval = hash(key);
  hashtable_entry* current = hashtable[hashval];
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
ssize_t hash_set(const char* const key, const char* const value) {
  // Check to see if the key is already in the table; if
  // so, dump the old value and install the new one.
  size_t hashval = hash(key);
  hashtable_entry* current = hashtable[hashval];
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
  hashtable_entry* hte = create_hashtable_entry(key, value);
  if (hte == NULL) {
    return -1;
  }
  hte->next = hashtable[hashval];
  hashtable[hashval] = hte;
  return 0;
}

/*
 * hash(): implements djb2 hashing.
 * source: http://www.cse.yorku.ca/~oz/hash.html
 */
size_t hash(const char* str) {
  size_t hash = 5381;
  unsigned char c;

  while ((c = (unsigned char)(*str++))) {
    /* hash * 33 + c */
    hash = ((hash << 5) + hash) + c;
  }

  return hash % DEFAULT_HT_SIZE;
}

/*
 * flush_hashtable(): write every entry in the hashtable to a
 * file on disk
 */
void flush_hashtable(const char* const path) { (void)path; }

/*
 * restore_hashtable(): parse serialzied hashtable file
 * and restore entries in memory.
 */
void restore_hashtable(const char* const path) { (void)path; }

/*
 * clear_hashtable(): prior to shutdown, walk the hashtable and
 * free all entries. This is implemented mostly for diagnostic purposes.
 */
void clear_hashtable() {
  for (int i = 0; i < DEFAULT_HT_SIZE; i++) {
    if (hashtable[i] != NULL) {
      hashtable_entry* current = hashtable[i];
      hashtable_entry* next = NULL;
      while (current != NULL) {
        if (current->next != NULL) {
          next = current->next;
        }
        free_hashtable_entry(current);
        current = next;
      }
    }
  }
}
