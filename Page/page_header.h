#pragma once

#include<cstdint>
#include <utility>


struct PageHeader {
    uint32_t page_id;             // 0-3
    uint32_t lsn;                 // 4-7
    uint32_t num_of_tuples;       // 8-11

    uint16_t free_space_start;    // 12-13
    uint16_t free_space_end;      // 14-15
    
    // No need to make array of slots, as size is not known. keeping inserting Slot as data grows
    // starts from 16 bytes
};
