#pragma once

#include "common.h"

typedef struct flag_settings {
  char* address;
  size_t port;
} flag_settings;

#define DEFAULT_HT_SIZE 1000

typedef struct hashtable_entry {
  char* key;
  char* value;
  struct hashtable_entry* next;
} hashtable_entry;

extern hashtable_entry* hashtable[];

// hashtable.c
char* hash_get(const char* const key);
ssize_t hash_set(const char* const key, const char* const value);
size_t hash(const char* str);
void free_hashtable_entry(hashtable_entry* hte);
hashtable_entry* create_hashtable_entry(const char* const key,
                                        const char* const value);
void flush_hashtable(const char* const path);
void free_hashtable(void);

// init.c
flag_settings* parse_flags(const int argc, char* const* const argv);
ssize_t initialize_server(flag_settings* flags, net_config* config);
