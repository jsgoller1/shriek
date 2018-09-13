#pragma once

#include "shriek_types.h"

serialized_message* serialize(const message* const message_data);
message* deserialize(const serialized_message* const s_message);
