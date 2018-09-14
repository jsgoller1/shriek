#pragma once

#include "shriek_types.h"

serialized_message* alloc_serialized_message(const int connection_id,
                                             char* const data,
                                             const size_t len);
void free_serialized_message(serialized_message* s_message);

serialized_message* serialize(message* const message_data);
message* deserialize(const serialized_message* const s_message);
