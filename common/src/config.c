/*
 * config.c - a library of for parsing CLI flags and setting configuration
 */
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "shriek_types.h"

/*
 * alloc_config(): configuration constructor
 */
configuration* alloc_configuration() {
  configuration* config = calloc(1, sizeof(configuration));
  if (config == NULL) {
    fprintf(stderr, "alloc_config() | Memory error. Quitting...");
    return NULL;
  }
  return config;
}

/*
 * free_config(): configuration destructor
 */
void free_configuration(configuration* config) {
  free(config->address);
  free(config->port);
  free(config);
}

/*
 * parse_flags(): handle reading read CLI flags and constructing settings
 * dict for initialization; initializer functions should handle quitting
 * if invalid / insufficient flags are passed or providing default values.
 */
configuration* parse_flags(const int argc, char* const* const argv) {
  (void)argc;
  (void)argv;

  configuration* config = alloc_configuration();
  if (config == NULL) {
    return NULL;
  }

  return config;
}
