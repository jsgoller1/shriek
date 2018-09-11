#pragma once
#include <stdio.h>

#include "common.h"

#define MAX_LINE MAX_KEY_SIZE + MAX_VAL_SIZE

typedef struct flag_settings {
  char* address;
  size_t port;
} flag_settings;

flag_settings* parse_flags(const int argc, char* const* const argv);
ssize_t initialize_client(const flag_settings* const flags, net_config** config,
                          char** linep, char** key, char** value);
void cleanup(net_config* config, char* linep, char* key, char* value);
