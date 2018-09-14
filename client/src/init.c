/*
 * init.c - setup / teardown library for Shriek client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "config.h"
#include "net.h"
#include "shriek_types.h"

/*
 * initialize_client(): Set up all necessary data structures for client
 * operation
 */
ssize_t initialize_client(configuration* const config, char** linep, char** key,
                          char** value) {
  // Initialize default values
  if (config->address == NULL) {
    config->address = strdup("127.0.0.1");
  }

  if (config->port == NULL) {
    config->port = strdup("9000");
  }

  if (config->max_connections == NULL) {
    config->max_connections = MAX_CONNECTIONS;
  }

  // Initialize remaining data for client
  *linep = malloc(sizeof(char) * MAX_LINE);
  *key = malloc(sizeof(char) * MAX_KEY_SIZE);
  *value = malloc(sizeof(char) * MAX_VAL_SIZE);

  // Null pointer checks
  if (!(config->address && config->port && *linep && *key && *value)) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup_client(config, *linep, *key, *value);
    return -1;
  }

  // Initiate connection to server
  if (!(initialize_connection_pool(1) &&
        node_connect(config->address, config->port))) {
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

  return 0;
}

/*
 * cleanup_client(): Ensures all client structures are properly freed
 * prior to shutdown
 */
void cleanup_client(configuration* config, char* linep, char* key,
                    char* value) {
  free_configuration(config);
  cleanup_connection_pool();
  free(linep);
  free(key);
  free(value);
}
