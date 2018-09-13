#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages.h"
#include "net.h"
#include "serialization.h"
#include "shriek_types.h"

message* create_message(enum action_type action, const char* const key,
                        const char* const value) {
  message* message_data = malloc(sizeof(message));
  if (message_data == NULL) {
    fprintf(stderr,
            "create_message() | Memory error while creating message.\n");
    return NULL;
  }
  memset(message_data, '\0', sizeof(message));

  message_data->action = action;
  if (key != NULL) {
    message_data->key_size = strlen(key);
    message_data->key = strdup(key);
  } else {
    message_data->key_size = 0;
    message_data->key = NULL;
  }

  if (value != NULL) {
    message_data->value_size = strlen(value);
    message_data->value = strdup(value);
  } else {
    message_data->value_size = 0;
    message_data->value = NULL;
  }

  return message_data;
}

ssize_t send_message(const net_config* const config_data,
                     const enum action_type action, const char* const key,
                     const char* const value) {
  printf("send_message() | sending message %d %s %s\n", action, key, value);

  message* message_data = create_message(action, key, value);
  if (message_data == NULL) {
    return -1;
  }

  serialized_message* s_message = serialize(message_data);
  if (s_message == NULL) {
    free(message_data);
    return -1;
  }

  char* reply = send_data(config_data, s_message);

  fprintf(stdout, "send_message() | server reply: %s\n", reply);
  free_message(message_data);
  free(s_message->data);
  free(s_message);
  return 0;
}

message* listen_for_messages(const net_config* const config) {
  serialized_message* s_message = recv_data(config);
  if (s_message == NULL) {
    return NULL;
  }

  message* message_data = deserialize(s_message);
  free(s_message->data);
  free(s_message);
  return message_data;
}

void free_message(message* message_data) {
  free(message_data->key);
  free(message_data->value);
  free(message_data);
}
