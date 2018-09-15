#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "messages.h"
#include "server.h"
#include "shriek_types.h"

/*
 * main() - entrypoint for Shriek server
 */
int main(int argc, char** argv) {
  hashtable_entry** ht = NULL;
  message *message_data = NULL, *reply = NULL;
  char *reply_key = NULL, *reply_value = NULL;

  // Setup
  configuration* config = parse_flags(argc, argv);
  if (config == NULL) {
    return -1;
  }

  if (initialize_server(config, &ht) == -1) {
    return -1;
  }

  printf("Started Shriek server on %s:%s.\n", config->address, config->port);
  while ((message_data = recv_message()) != NULL) {
    if (message_data->action == GET) {
      reply_key = message_data->key;
      reply_value = hash_get(ht, message_data->key);
    } else if (message_data->action == SET) {
      hash_set(ht, message_data->key, message_data->value);
      reply_key = "0";
      reply_value = "0";
    } else {
      reply_key = "ERROR";
      reply_value = "ERROR";
    }
    reply = alloc_message(REPLY, message_data->connection_id, reply_key,
                          reply_value);
    send_message(reply);

    free_message(reply);
    free_message(message_data);
  }

  cleanup_server(config, ht);
  return 0;
}
