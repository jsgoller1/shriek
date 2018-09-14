/*
 * init.c - setup / teardown library for Shriek client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "config.h"
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

  // TODO: Client fails if server doesn't exist, which is intended behavior
  // but not helpful until networking in the server is actually implemented.
  //

  // Initiate connection to server
  /*
    if (connect_to_server(*config) == -1) {
      cleanup(*config, *linep, *key, *value);
      return -1;
    }
  */

  return 0;
}

/*
 * cleanup_client(): Ensures all client structures are properly freed
 * prior to shutdown
 */
void cleanup_client(configuration* config, char* linep, char* key,
                    char* value) {
  free_configuration(config);
  free(linep);
  free(key);
  free(value);
}
