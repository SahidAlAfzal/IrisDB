#pragma once

#include<cstdint>

#pragma pack(push,1) // Force 1-byte alignment (no padding)
struct Slot {                      // 6 bytes (checked)   => after pack - 5 bytes (no wastage of memory)
    uint16_t tuple_offset;
    uint16_t tuple_size;
    bool is_deleted;
};
#pragma pack(pop)


// Now sizeof(Slot) is guaranteed to be exactly 5 bytes on every machine.
