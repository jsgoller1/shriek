#pragma once

#include "shriek_types.h"

extern connection_pool* pool;

ssize_t initialize_connection_pool(const size_t size);
void cleanup_connection_pool();
ssize_t pool_add(const int socket_fd, bool listening);
void pool_remove(const int socket_fd);
