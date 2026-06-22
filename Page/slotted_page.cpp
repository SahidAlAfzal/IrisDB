#include<cstdint>
#include<cstring>
#include<utility>
#include "slotted_page.h"

const int PAGE_SIZE = 4096;

PageHeader* SlottedPage::GetHeader() {
    return reinterpret_cast<PageHeader*>(this->page_data);
}

Slot* SlottedPage::GetSlot(uint16_t slot_id) {
    return reinterpret_cast<Slot*>(
        page_data + sizeof(PageHeader)
    ) + slot_id;
}



SlottedPage::SlottedPage(char* buffer_pool_frame){
    this->page_data = buffer_pool_frame;
}

void SlottedPage::Init(int page_id){
    memset(page_data, 0, PAGE_SIZE);
    PageHeader* header = GetHeader();
    header->page_id = page_id;                                // 0-3
    header->lsn = 0;                                         // 4-7
    header->num_of_tuples = 0;                              // 8-11

    header->free_space_start = PAGE_SIZE;                  // 12-13
    header->free_space_end = sizeof(PageHeader);          // size of header is 16 bytes (0-15)
}


bool SlottedPage::InsertTuple(const char* tuple_data, uint16_t tuple_size){
    // 1. Checks if there is enough free space in the middle gap. (If not, return false).
    // new insert = 1 Slot(6) + tuple_data(depends)
    PageHeader* header = reinterpret_cast<PageHeader*>(this->page_data); 

    int free_space = header->free_space_start - header->free_space_end;
    if(free_space < (sizeof(Slot) + tuple_size)) return false;

    

    Slot* new_slot = GetSlot(header->num_of_tuples);

    new_slot->tuple_offset = header->free_space_start - tuple_size;
    new_slot->tuple_size = tuple_size;

    // write the tuple_data in the end
    memcpy(page_data + new_slot->tuple_offset,
           tuple_data,
           tuple_size
    );

    header->num_of_tuples++;

    // update boundary
    header->free_space_start -= tuple_size;
    header->free_space_end += sizeof(Slot);

    return true;
}



bool SlottedPage::deleteTuple(uint16_t slot_id){
    PageHeader* header = GetHeader();
    
    // 1. Boundary check!
    if (slot_id >= header->num_of_tuples) {
        return false; 
    }


    // 2. Mark delete 
    Slot* target_slot = GetSlot(slot_id);
    if(target_slot->is_deleted == true) return false;   // already deleted

    target_slot->is_deleted = true;
    return true;
}


char* SlottedPage::getTuple(u_int16_t slot_id){
    PageHeader* header = GetHeader();
    
    // 1. Boundary check!
    if (slot_id >= header->num_of_tuples) {
        return nullptr; 
    }


    // 2. Find the slot
    Slot* target_slot = GetSlot(slot_id);
    if(target_slot->is_deleted == true) return nullptr;

    //char* tuple_data = reinterpret_cast<char*>(page_data + target_slot->tuple_offset);

    return page_data + target_slot->tuple_offset;
}


// SlottedPage(int page_id);
//     bool InsertTuple(const char* tuple_data, uint16_t tuple_size);
//     bool deleteTuple(uint16_t slot_id);
//     bool getTuple(u_int16_t slot_id);
//     PageHeader* GetHeader();
//     Slot* GetSlot(uint16_t slot_id);