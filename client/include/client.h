#pragma once

#include <stdio.h>

#include "shriek_types.h"

#define MAX_LINE MAX_KEY_SIZE + MAX_VAL_SIZE

configuration* parse_client_flags(const int argc, char* const* const argv);
ssize_t initialize_client(configuration** const config, char** linep,
                          char** key, char** value);
void cleanup(configuration* config, char* linep, char* key, char* value);
