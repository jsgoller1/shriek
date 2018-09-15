#pragma once

#include "hashtable.h"
#include "shriek_types.h"

ssize_t initialize_server(configuration* config, hashtable_entry*** ht);
void cleanup_server(configuration* config, hashtable_entry** ht);
