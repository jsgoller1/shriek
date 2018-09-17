#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "log.h"
#include "messages.h"
#include "server.h"
#include "shriek_types.h"

/*
 * main() - entrypoint for Shriek server
 */
int main(int argc, char** argv) {
  hashtable_entry** ht = NULL;
  message* new_message = NULL;
  char *reply_key = NULL, *reply_value = NULL;

  // Setup
  configuration* config = parse_flags(argc, argv);
  if (config == NULL) {
    return -1;
  }

  if (initialize_server(config, &ht) == -1) {
    return -1;
  }

  log_info("Started Shriek server on %s:%s.", config->address, config->port);
  while ((new_message = recv_message())) {
    if (new_message == NULL) {
      continue;
    }
    if (new_message->action == GET) {
      reply_value = hash_get(ht, new_message->key);
      log_trace("Looked up %s, got %s", new_message->key, reply_value);
      send_message(REPLY, new_message->connection_id, new_message->key,
                   reply_value);
    } else if (new_message->action == SET) {
      hash_set(ht, new_message->key, new_message->value);
      reply_key = "0";
      send_message(REPLY, new_message->connection_id, reply_key, NULL);
      log_trace("Set %s to %s", new_message->key, new_message->value);
    }
    free_message(new_message);
  }

  cleanup_server(config, ht);
  return 0;
}
