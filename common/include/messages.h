#pragma once

#include "shriek_types.h"

message* alloc_message(enum action_type action, const char* const key,
                       const char* const value);
void free_message(message* message_data);
ssize_t send_message(const size_t connection_id, const enum action_type action,
                     const char* const key, const char* const value);
ssize_t reply_message(const message* const message_data,
                      const char* const reply_data);
message* recv_message(const configuration* const config);
