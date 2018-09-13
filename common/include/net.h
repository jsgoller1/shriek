#pragma once

#include "shriek_types.h"

ssize_t connect_to_server(configuration* const config);
char* send_data(const configuration* const config,
                const serialized_message* const s_message);
serialized_message* recv_data(const configuration* const config);
ssize_t disconnect(const configuration* const config);
