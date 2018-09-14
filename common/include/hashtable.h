#pragma once

#define DEFAULT_HT_SIZE 1000

typedef struct hashtable_entry {
  char* key;
  char* value;
  struct hashtable_entry* next;
} hashtable_entry;

// Hashtable management functions
hashtable_entry** alloc_hashtable(const size_t ht_size);
void free_hashtable(hashtable_entry** ht, const size_t ht_size);
ssize_t flush_hashtable(hashtable_entry* const* const ht,
                        const char* const path);
ssize_t restore_hashtable(hashtable_entry** const ht, const char* const path);

// Hashtable entry functions
hashtable_entry* alloc_hashtable_entry(const char* const key,
                                       const char* const value);
void free_hashtable_entry(hashtable_entry* hte);
char* hash_get(hashtable_entry** ht, const char* const key);
ssize_t hash_set(hashtable_entry** ht, const char* const key,
                 const char* const value);

size_t djb2_hash(const char* str);
