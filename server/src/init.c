/*
 * init.c - setup / teardown library for Shriek server
 */

#include <stdio.h>

#include "config.h"
#include "connection_pool.h"
#include "hashtable.h"
#include "server.h"
#include "shriek_types.h"
#include "sockets.h"

/*
 * initialize_server(): sets up the server and begins multiplexed
 * listen for incoming connections
 */
ssize_t initialize_server(configuration* const config, hashtable_entry*** ht) {
  if (config->hashtable_size == 0) {
    config->hashtable_size = DEFAULT_HT_SIZE;
  }

  *ht = alloc_hashtable(config->hashtable_size);
  if (*ht == NULL) {
    cleanup_server(config, *ht);
    return -1;
  }

  // Initiate listener
  if (initialize_connection_pool(MAX_CONNECTIONS) == -1) {
    fprintf(stderr,
            "initialize_server() | couldn't initialize connection pool.\n");
    cleanup_server(config, *ht);
    return -1;
  }

  if (socket_listen(config->address, config->port) == -1) {
    fprintf(stderr, "initialize_server() | couldn't open listening socket.\n");
    cleanup_server(config, *ht);
    return -1;
  }

  return 0;
}

/*
 * cleanup_server(): tear down server, prepare for shutdown.
 */
void cleanup_server(configuration* config, hashtable_entry** ht) {
  cleanup_connection_pool();
  flush_hashtable(ht, "shriek-serialized.bin");
  free_hashtable(ht, config->hashtable_size);
  free_configuration(config);
}
