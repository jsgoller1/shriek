#include <stdio.h>
#include <stdlib.h>

#include "messages.h"
#include "net.h"
#include "server.h"
#include "shriek_types.h"

/*
 * main() - entrypoint for server
 */
int main(int argc, char** argv) {
  message* message_data;
  configuration* config = parse_flags(argc, argv);

  if (initialize_server(config) == -1) {
    return -1;
  }

  printf("Starting shriek server...\n");
  while ((message_data = listen_for_messages(config)) != NULL) {
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
  clear_hashtable();
  return 0;
}
