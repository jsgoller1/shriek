#pragma once

#include "shriek_types.h"

message* alloc_message(enum action_type action, const int connection_id,
                       const char* const key, const char* const value);
void free_message(message* message_data);

ssize_t send_message(message* const message_data);
message* recv_message(void);
