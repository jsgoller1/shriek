#pragma once

#include "net.h"

configuration* parse_flags(const int argc, char* const* const argv);
ssize_t initialize_server(configuration* config);
