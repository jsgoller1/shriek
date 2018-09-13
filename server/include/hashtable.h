#define DEFAULT_HT_SIZE 1000

typedef struct hashtable_entry {
  char* key;
  char* value;
  struct hashtable_entry* next;
} hashtable_entry;

extern hashtable_entry* hashtable[];

char* hash_get(const char* const key);
ssize_t hash_set(const char* const key, const char* const value);
size_t hash(const char* str);
void free_hashtable_entry(hashtable_entry* hte);
hashtable_entry* create_hashtable_entry(const char* const key,
                                        const char* const value);
void flush_hashtable(const char* const path);
void free_hashtable(void);
