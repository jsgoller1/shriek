/*
 * init.c - setup / teardown library for Shriek client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "shriek_types.h"

/*
 * parse_flags_client() - parse argc/argv into configuration structure
 * needed for initialization
 */
configuration* parse_flags_client(const int argc, char* const* const argv) {
  (void)argc;
  (void)argv;
  return NULL;
}

/*
 * initialize_client(): Set up all necessary data structures for client
 * operation
 */
ssize_t initialize_client(configuration** const config, char** linep,
                          char** key, char** value) {
  // Initialize config struct
  *config = malloc(sizeof(configuration));
  if (*config == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    return -1;
  }

  // TODO: Parse addr/port out of the flag settings;
  // this should be handled by a parse_flags()
  // function, but for now, just hardcode to localhost
  (*config)->address = strdup("127.0.0.1");
  if ((*config)->address == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }
  (*config)->port = strdup("9000");
  if ((*config)->port == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

  // TODO: Client fails if server doesn't exist, which is intended behavior
  // but not helpful until networking in the server is actually implemented.
  //
  /*
    if (connect_to_server(*config) == -1) {
      cleanup(*config, *linep, *key, *value);
      return -1;
    }
  */

  // Initialize remaining data for client
  *linep = malloc(sizeof(char) * MAX_LINE);
  if (*linep == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

  *key = malloc(sizeof(char) * MAX_KEY_SIZE);
  if (*key == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

  *value = malloc(sizeof(char) * MAX_VAL_SIZE);
  if (*value == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

  return 0;
}

/*
 * cleanup_client(): Ensures all client structures are properly freed
 * prior to shutdown
 */
void cleanup(configuration* config, char* linep, char* key, char* value) {
  free(config->address);
  free(config->port);
  free(config);
  free(linep);
  free(key);
  free(value);
}
