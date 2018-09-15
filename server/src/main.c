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
  message* sent = NULL;
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
  while ((sent = recv_message()) != NULL) {
    if (sent->action == GET) {
      reply_value = hash_get(ht, sent->key);
      send_message(REPLY, sent->connection_id, sent->key, reply_value);
    } else if (sent->action == SET) {
      hash_set(ht, sent->key, sent->value);
      reply_key = "0";
      send_message(REPLY, sent->connection_id, reply_key, NULL);
    }
    free_message(sent);
  }

  cleanup_server(config, ht);
  return 0;
}
