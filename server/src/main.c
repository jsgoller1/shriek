#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "messages.h"
#include "net.h"
#include "server.h"
#include "shriek_types.h"

/*
 * main() - entrypoint for Shriek server
 */
int main(int argc, char** argv) {
  message* message_data = NULL;
  hashtable_entry** ht = NULL;
  configuration* config = parse_flags(argc, argv);

  if (initialize_server(config, ht) == -1) {
    return -1;
  }

  printf("Starting shriek server...\n");
  while ((message_data = recv_message()) != NULL) {
    if (message_data->action == GET) {
      printf("GET: %s - %s\n", message_data->key,
             hash_get(ht, message_data->key));
    } else if (message_data->action == SET) {
      hash_set(ht, message_data->key, message_data->value);
      printf("SET: %s - %s\n", message_data->key,
             hash_get(ht, message_data->key));
    } else {
      fprintf(stderr, "Invalid command.\n");
    }
    free_message(message_data);
  }

  cleanup_server(config, ht);
  return 0;
}
