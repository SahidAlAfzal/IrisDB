#ifndef HEAP_TABLE_H
#define HEAP_TABLE_H

#include "../Buffer/BufferPoolManager.h"
#include "../DataRepresentationLayer/include/tuple.h"
#include "../indexing/record_id.h"


class HeapTable {
private:
    BufferPoolManager* bpm_;
    int first_page_id_;      // head of the dll
    int last_page_id_;       // tail of the dll

public:
    // bootstrap a brand new empty heap table
    HeapTable(BufferPoolManager* bpm);

    // Loads an existing heap table
    HeapTable(BufferPoolManager* bpm, int first_page_id, int last_page_id);

    // It will use InsertTuple api of SlottedPage to insert and return rid
    bool InsertTuple(const Tuple& tuple, RecordID* rid);

    // input : record_id output: tuple
    bool GetTuple(const RecordID& rid, Tuple* tuple);

    int GetFirstPageId() const { return first_page_id_; }
};


#endif
