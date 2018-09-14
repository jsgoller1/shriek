#pragma once

#include "shriek_types.h"

ssize_t initialize_connection_pool(const size_t size);
void cleanup_connection_pool(void);
ssize_t pool_add(const int socket_fd, bool listening);
void pool_remove(const int socket_fd);
serialized_message* pool_listen(void);
ssize_t pool_send(size_t connection_id, serialized_message* s_message);
