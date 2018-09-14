/*
 * messages.c - high level library for sending and receiving
 * Shriek application protocol messages. See PROTOCOL.md
 * for a description of the Shriek protocol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection_pool.h"
#include "messages.h"
#include "serialization.h"
#include "shriek_types.h"

/*
 * alloc_message(): message constructor
 */
message* alloc_message(enum action_type action, const char* const key,
                       const char* const value) {
  message* message_data = calloc(1, sizeof(message));
  if (message_data == NULL) {
    fprintf(stderr,
            "create_message() | Memory error while creating message.\n");
    return NULL;
  }

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

/*
 * free_message(): message destructor
 */
void free_message(message* message_data) {
  free(message_data->key);
  free(message_data->value);
  free(message_data);
}

/*
 * send_message(): serialize a message structure, send it
 * to a connected host.
 */
ssize_t send_message(const size_t connection_id, const enum action_type action,
                     const char* const key, const char* const value) {
  printf("send_message() | sending message %d %s %s\n", action, key, value);

  message* message_data = alloc_message(action, key, value);
  if (message_data == NULL) {
    return -1;
  }

  serialized_message* s_message = serialize(message_data);
  if (s_message == NULL) {
    free(message_data);
    return -1;
  }

  ssize_t res = pool_send(connection_id, s_message);

  fprintf(stdout, "send_message() | server reply: %ld\n", res);
  free_message(message_data);
  free(s_message->data);
  free(s_message);
  return 0;
}

/*
 * recv_message(): wait for a message to arrive, and return
 * it deserialized if so. Note that because connections are
 * multiplexed, multiple messages may arrive, so make sure
 * to check the next_message pointer!
 */
message* recv_message() {
  serialized_message* s_message = pool_listen();
  if (s_message == NULL) {
    return NULL;
  }

  message* message_data = deserialize(s_message);
  free(s_message->data);
  free(s_message);
  return message_data;
}
