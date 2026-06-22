#ifndef SLOTTED_PAGE_H
#define SLOTTED_PAGE_H


#include<cstdint>
#include<utility>
#include "Slot.h"
#include "page_header.h"

const int PAGE_SIZE = 4096;

class SlottedPage {
private:
    char* page_data;

public:
    SlottedPage(char* buffer_pool_frame);
    void Init(int page_id);
    bool InsertTuple(const char* tuple_data, uint16_t tuple_size);
    bool deleteTuple(uint16_t slot_id);
    char* getTuple(u_int16_t slot_id);
    PageHeader* GetHeader();
    Slot* GetSlot(uint16_t slot_id);
};







#endif