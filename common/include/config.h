#pragma once

#include "shriek_types.h"

configuration* alloc_configuration(void);
void free_configuration(configuration* config);
configuration* parse_flags(const int argc, char* const* const argv);
