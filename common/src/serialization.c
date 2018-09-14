/*
 * serialization.c - helper functions for converting a Shriek message
 * struct into over-the-wire binary and vice-versa.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages.h"
#include "serialization.h"
#include "shriek_types.h"

/*
 * alloc_serialized_message() - serialized_message constructor
 */
serialized_message* alloc_serialized_message(const size_t len) {
  serialized_message* s_message = calloc(1, sizeof(serialized_message));
  if (s_message == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...\n");
    return NULL;
  }
  s_message->len = len;

  s_message->data = calloc(1, s_message->len);
  if (s_message->data == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...\n");
    free(s_message);
    return NULL;
  }

  return s_message;
}

/*
 * free_serialized_message() - serialized_message destructor
 */
void free_serialized_message(serialized_message* s_message) {
  free(s_message->data);
  free(s_message);
}

/*
 * serialize() - convert a message to a serialized binary message
 */
serialized_message* serialize(const message* const message_data) {
  // Initialized serialized message and message buffer
  serialized_message* s_message = alloc_serialized_message(
      (sizeof(size_t) * 3) +
      sizeof(char) * (message_data->key_size + message_data->value_size));

  // Copy action and size fields to string buffer
  memcpy(s_message->data, &(message_data->action), sizeof(size_t));
  memcpy(s_message->data + sizeof(size_t), &(message_data->key_size),
         sizeof(size_t));
  memcpy(s_message->data + (sizeof(size_t) * 2), &(message_data->value_size),
         sizeof(size_t));

  // Copy key data to buffer
  if (message_data->key != NULL) {
    memcpy(s_message->data + (sizeof(size_t) * 3), message_data->key,
           message_data->key_size);
  }

  // Copy value data to buffer
  if (message_data->value != NULL) {
    memcpy(s_message->data + ((sizeof(size_t) * 3) + message_data->key_size),
           message_data->value, message_data->value_size);
  }

  return s_message;
}

/*
 * deserialize() - convert a serialized message to a message struct
 */
message* deserialize(const serialized_message* const s_message) {
  enum action_type action = 0;
  size_t key_size = 0;
  size_t value_size = 0;

  memcpy(&action, s_message->data, sizeof(enum action_type));
  memcpy(&key_size, s_message->data + sizeof(size_t), sizeof(size_t));
  memcpy(&value_size, s_message->data + (sizeof(size_t) * 2), sizeof(size_t));

  char* key = calloc(1, sizeof(char) * (key_size + 1));
  if (key == NULL) {
    fprintf(stderr, "deserialize() | Memory error.\n");
    return NULL;
  }
  memcpy(key, s_message->data + sizeof(size_t) * 3, key_size);

  char* value = calloc(1, sizeof(char) * (value_size + 1));
  if (value == NULL) {
    fprintf(stderr, "deserialize() | Memory error.\n");
    free(key);
    return NULL;
  }
  memcpy(value, s_message->data + sizeof(size_t) * 3 + key_size, value_size);
  printf("deserialize() | %s - %s\n", key, value);

  message* message_data = alloc_message(action, key, value);

  free(key);
  free(value);
  return message_data;
}
