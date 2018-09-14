/*
 * init.c - setup / teardown library for Shriek server
 */

#include <stdio.h>

#include "hashtable.h"
#include "server.h"
#include "shriek_types.h"

/*
 * initialize_server(): sets up the server and begins multiplexed
 * listen for incoming connections
 */
ssize_t initialize_server(configuration* config, hashtable_entry** ht) {
  (void)ht;
  (void)config;
  return 0;
}

/*
 * cleanup_server(): tear down server, prepare for shutdown.
 */
void cleanup_server(configuration* config, hashtable_entry** ht) {
  flush_hashtable(ht, "shriek-serialized.bin");
  free_hashtable(ht, config->hashtable_size);
}
