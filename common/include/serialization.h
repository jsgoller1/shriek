#pragma once

#include "shriek_types.h"

serialized_message* alloc_serialized_message(const size_t size);
void free_serialized_message(serialized_message* s_message);
serialized_message* serialize(const message* const message_data);
message* deserialize(const serialized_message* const s_message);
