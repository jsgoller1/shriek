#pragma once
#pragma clang diagnostic ignored "-Wpadded"

#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

#define MAX_KEY_SIZE KB(10)
#define MAX_VAL_SIZE KB(100)

enum action_type { SET, GET };

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

typedef struct net_config {
  char* address;
  size_t port;
  size_t socket;
} net_config;

// net.c
ssize_t connect_to_server(net_config* const config);
char* send_data(const net_config* const config,
                const serialized_message* const s_message);
ssize_t disconnect(const net_config* const config);

// serialization.c
serialized_message* serialize(const message* const message_data);
message* deserialize(const char* const serialized_message);

// messages.c
message* create_message(enum action_type action, const char* const key,
                        const char* const val);
ssize_t send_message(const net_config* const config_data,
                     const enum action_type action, const char* const key,
                     const char* const value);
void free_message(message* message_data);
