#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "common.h"

flag_settings* parse_flags(const int argc, char* const* const argv) {
  (void)argc;
  (void)argv;
  return NULL;
}

ssize_t initialize_client(const flag_settings* const flags, net_config** config,
                          char** linep, char** key, char** value) {
  // Initialize config struct
  *config = malloc(sizeof(net_config));
  if (*config == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    return -1;
  }
  // TODO: Parse addr/port out of the flag settings
  (void)flags;
  (*config)->address = strdup("127.0.0.1");
  if ((*config)->address == NULL) {
    fprintf(stderr, "initialize_client() | Memory error. Quitting...");
    cleanup(*config, *linep, *key, *value);
    return -1;
  }
  (*config)->port = 9000;
  if (connect_to_server(*config) == -1) {
    cleanup(*config, *linep, *key, *value);
    return -1;
  }

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

// cleanup(): Ensures all data malloc'd is appropriately freed; freeing
// a NULL pointer is safe per the man pages
void cleanup(net_config* config, char* linep, char* key, char* value) {
  free(config->address);
  free(config);
  free(linep);
  free(key);
  free(value);
}
