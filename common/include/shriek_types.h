#pragma once
#pragma clang diagnostic ignored "-Wpadded"

#include <poll.h>
#include <stdbool.h>

#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 10
#endif

#ifndef MAX_CONNECTION_BACKLOG
#define MAX_CONNECTION_BACKLOG 10
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

typedef struct connection_pool {
  bool has_listener;
  size_t size;
  struct pollfd** connections;
};

typedef struct configuration {
  char* address;
  char* port;
  size_t hashtable_size;
  size_t max_connections;
} configuration;

typedef struct message {
  enum action_type action;
  size_t key_size;
  size_t value_size;
  char* key;
  char* value;
  struct message* next_message;
} message;

typedef struct serialized_message {
  size_t len;
  char* data;
} serialized_message;
