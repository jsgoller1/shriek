/*
 * init.c - setup / teardown library for Shriek server
 */

#include <stdio.h>

#include "server.h"

/*
 * parse_flags(): handle reading read CLI flags and constructing settings
 * dict for server initialization
 */
configuration* parse_flags_server(const int argc, char* const* const argv) {
  (void)argc;
  (void)argv;
  return NULL;
}

/*
 * initialize_server(): sets up the server and begins multiplexed
 * listen for incoming connections
 */
ssize_t initialize_server(configuration* config, hashtable_entry** ht) {
  (void)flags;
  (void)config;
  return 0;
}

/*
 * cleanup_server(): tear down server, prepare for shutdown.
 */
void cleanup_server(configuration* config, hashtable** ht) {
  flush_hashtable(ht, "shriek-serialized.bin");
  free_hashtable(ht, config->hashtable_size);
}
