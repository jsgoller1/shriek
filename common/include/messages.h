#pragma once

#include "shriek_types.h"

message* create_message(enum action_type action, const char* const key,
                        const char* const val);
ssize_t send_message(const net_config* const config,
                     const enum action_type action, const char* const key,
                     const char* const value);
message* listen_for_messages(const net_config* const config);
void free_message(message* message_data);
