#pragma once

#include "shriek_types.h"

extern connection_pool* pool;

ssize_t node_connect(configuration* const config);
ssize_t node_disconnect(const configuration* const config);

char* send_data(const size_t connection_id,
                const serialized_message* const s_message);
serialized_message* recv_data(void);
