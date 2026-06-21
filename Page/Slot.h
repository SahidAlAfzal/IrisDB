#pragma once

#include<cstdint>

struct Slot {                      // 6 bytes (checked)
    uint16_t tuple_offset;
    uint16_t tuple_size;
    bool is_deleted;
};
