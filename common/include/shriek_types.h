#pragma once
#pragma clang diagnostic ignored "-Wpadded"

#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

#define MAX_KEY_SIZE KB(10)
#define MAX_VAL_SIZE KB(100)

enum action_type {
  REPLY,
  SET,
  GET,
};

typedef struct net_config {
  char* address;
  char* port;
  int socket;
} net_config;

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
