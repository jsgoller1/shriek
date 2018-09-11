#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

serialized_message* serialize(const message* const message_data) {
  // Initialized serialized message and message buffer
  serialized_message* s_message = malloc(sizeof(serialized_message));
  if (s_message == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...\n");
    return NULL;
  }
  memset(s_message, '\0', sizeof(serialized_message));
  s_message->len =
      (sizeof(size_t) * 3) +
      sizeof(char) * (message_data->key_size + message_data->value_size);

  char* buffer = malloc(s_message->len);
  if (buffer == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...\n");
    free(s_message);
    return NULL;
  }
  memset(buffer, '\0', s_message->len);

  // Copy action and size fields to string buffer
  memcpy(buffer, &(message_data->action), sizeof(size_t));
  memcpy(buffer + sizeof(size_t), &(message_data->key_size), sizeof(size_t));
  memcpy(buffer + (sizeof(size_t) * 2), &(message_data->value_size),
         sizeof(size_t));

  // Copy key data to buffer
  if (message_data->key != NULL) {
    memcpy(buffer + (sizeof(size_t) * 3), message_data->key,
           message_data->key_size);
  }

  // Copy value data to buffer
  if (message_data->value != NULL) {
    memcpy(buffer + ((sizeof(size_t) * 3) + message_data->key_size),
           message_data->value, message_data->value_size);
  }

  s_message->data = buffer;
  return s_message;
}

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

  message* message_data = create_message(action, key, value);

  free(key);
  free(value);
  return message_data;
}
