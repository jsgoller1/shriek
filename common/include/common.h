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
  char* port;
  int socket;
} net_config;

// net.c
ssize_t connect_to_server(net_config* const config);
char* send_data(const net_config* const config,
                const serialized_message* const s_message);
serialized_message* recv_data(const net_config* const config);
ssize_t disconnect(const net_config* const config);

// serialization.c
serialized_message* serialize(const message* const message_data);
message* deserialize(const serialized_message* const s_message);

// messages.c
message* create_message(enum action_type action, const char* const key,
                        const char* const val);
ssize_t send_message(const net_config* const config,
                     const enum action_type action, const char* const key,
                     const char* const value);
message* listen_for_messages(const net_config* const config);
void free_message(message* message_data);
