#pragma once
#pragma clang diagnostic ignored "-Wpadded"

#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 10
#endif

#ifndef MAX_KEY_SIZE
#define MAX_KEY_SIZE KB(10)
#endif

#ifndef MAX_VAL_SIZE
#define MAX_VAL_SIZE KB(100)
#endif

enum action_type {
  REPLY,
  SET,
  GET,
};

typedef struct configuration {
  char* address;
  char* port;
  size_t hashtable_size;
} configuration;

typedef struct message {
  enum action_type action;
  size_t key_size;
  size_t value_size;
  char* key;
  char* value;
} message;

typedef struct serialized_message {
  size_t len;
  char* data;
} serialized_message;
