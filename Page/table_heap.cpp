#include "table_heap.h"
#include "slotted_page.h"
#include <stdexcept>


HeapTable::HeapTable(BufferPoolManager* bpm) : bpm_(bpm) {
    Page* raw_page = bpm_->NewPage(&first_page_id_);

    if(raw_page == nullptr) {
        throw std::runtime_error("Buffer Pool is out of memory");
    }

    SlottedPage* first_page = new SlottedPage(raw_page->data);
    // Initialize linked list pointers (-1 means no page)
    first_page->GetHeader()->next_page_id = -1;
    first_page->GetHeader()->prev_page_id = -1;

    last_page_id_ = first_page_id_;
    bpm_->UnpinPage(first_page_id_, true); // just initialize so dirty
    delete first_page;
}


HeapTable::HeapTable(BufferPoolManager* bpm, int first_page_id, int last_page_id)
    : bpm_(bpm), first_page_id_(first_page_id), last_page_id_(last_page_id) {}

bool HeapTable::InsertTuple(const Tuple& tuple, RecordID* rid) {
    Page* raw_page = bpm_->FetchPage(last_page_id_);

    if(raw_page == nullptr) {
        // something wrong
        return false;
    }

    SlottedPage* current_page = new SlottedPage(raw_page->data);

    // scenario 1: There is space available for insertion of tuple in the current page
    if(current_page->InsertTuple(tuple.GetData(), tuple.GetLength())) {
        int slot_id = current_page->GetHeader()->num_of_tuples - 1;
        //int page_id = current_page->GetHeader()->page_id;

        *rid = RecordID(last_page_id_, slot_id);

        bpm_->UnpinPage(last_page_id_, true); // Dirty! data has been added.
        delete current_page;
        return true;
    }


    // scenario 2: No space left at last page
    int new_page_id;
    Page* new_raw_page = bpm_->NewPage(&new_page_id);

    if(new_raw_page == nullptr) {
        bpm_->UnpinPage(last_page_id_, false);
        delete current_page;
        return false;
    }

    SlottedPage* new_page = new SlottedPage(new_raw_page->data);
    new_page->Init(new_page_id);

    // Setup the doubly-linked list
    new_page->GetHeader()->prev_page_id = last_page_id_;
    new_page->GetHeader()->next_page_id = -1;
    current_page->GetHeader()->next_page_id = new_page_id;

    // Now insert it
    new_page->InsertTuple(tuple.GetData(), tuple.GetLength());

    // RecordID
    int slot_id = new_page->GetHeader()->num_of_tuples - 1;
    *rid = RecordID(new_page_id, slot_id);

    // Update the Table Heap's internal tracker
    int old_last_page_id = last_page_id_;
    last_page_id_ = new_page_id;

    // unpin the two pages
    bpm_->UnpinPage(old_last_page_id, true);      // next_page_id updated
    bpm_->UnpinPage(last_page_id_, true);         // new page and it got new tuple => Initialized


    delete current_page;
    delete new_page;

    return true;
}



bool TableHeap::GetTuple(const RecordID& rid, Tuple* tuple) {
    Page* raw_page = bpm_->FetchPage(rid.page_id_);
    if (raw_page == nullptr) return false;

    // Create wrapper on the stack (no need for 'new' here)
    SlottedPage current_page(raw_page->data);

    char* tuple_data = current_page.getTuple(rid.slot_id_);
    if (tuple_data == nullptr) {
        bpm_->UnpinPage(rid.page_id_, false);
        return false; // Tuple was deleted or doesn't exist
    }

    // Fetch the specific size of this tuple from the SlottedPage's Slot Array
    Slot* slot = current_page.GetSlot(rid.slot_id_);
    uint16_t tuple_size = slot->tuple_size;

    // Utilize the zero-copy deserialization constructor you built
    *tuple = Tuple(tuple_data, tuple_size);

    bpm_->UnpinPage(rid.page_id_, false); // Not dirty, just reading
    return true;
}
