#pragma once

#include <stdio.h>

#include "shriek_types.h"

#define MAX_LINE MAX_KEY_SIZE + MAX_VAL_SIZE
#define SERVER_CONNECTION_ID 1

configuration* parse_flags_client(const int argc, char* const* const argv);
ssize_t initialize_client(configuration* const config, char** linep, char** key,
                          char** value);
void cleanup_client(configuration* config, char* linep, char* key, char* value);
