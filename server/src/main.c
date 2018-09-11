#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "server.h"

int main(int argc, char** argv) {
  printf("Starting shriek server...\n");
  message* message_data;
  net_config* config = NULL;

  flag_settings* flags = parse_flags(argc, argv);

  if (initialize_server(flags, config) == -1) {
    return -1;
  }

  while ((message_data = listen_for_messages(config)) != NULL) {
    // TODO: Send reply over socket; for now, just print
    if (message_data->action == GET) {
      printf("GET: %s - %s\n", message_data->key, hash_get(message_data->key));
    } else if (message_data->action == SET) {
      hash_set(message_data->key, message_data->value);
      printf("SET: %s - %s\n", message_data->key, hash_get(message_data->key));
    } else {
      fprintf(stderr, "Invalid command.\n");
    }
    free_message(message_data);
  }

  flush_hashtable("shriek-serialized.bin");
  free_hashtable();
  return 0;
}
