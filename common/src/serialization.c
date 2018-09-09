#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

serialized_message* serialize(const message* const message_data) {
  // Initialized serialized message and message buffer
  serialized_message* s_message = malloc(sizeof(serialized_message));
  if (s_message == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...");
    return NULL;
  }
  memset(s_message, '\0', sizeof(serialized_message));
  s_message->len =
      (sizeof(size_t) * 3) +
      sizeof(char) * (message_data->key_size + message_data->value_size);

  char* buffer = malloc(s_message->len);
  if (buffer == NULL) {
    fprintf(stderr, "serialize() | Memory error. Quitting...");
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

message* deserialize(const char* const serialized_message) {
  (void)serialized_message;
  return NULL;
}
