#pragma once

#include "shriek_types.h"

ssize_t connect_to_server(net_config* const config);
char* send_data(const net_config* const config,
                const serialized_message* const s_message);
serialized_message* recv_data(const net_config* const config);
ssize_t disconnect(const net_config* const config);
