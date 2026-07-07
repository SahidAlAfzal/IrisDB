#ifndef B_PLUS_LEAF_PAGE_H
#define B_PLUS_LEAF_PAGE_H

#include <utility>    // for pair
#include "b_plus_tree_page.h"
#include "record_id.h"

#define MappingType std::pair<int, RecordID>   // key -> {page_id, slot_id}
/*
Inherited Base Header: 24 bytes
Leaf Header (next_page_id): 4 bytes (Leaf nodes act as a Linked List for fast range queries).
Total Header: 28 bytes.

Remaining Space: 4068 bytes.
Size of Pair (int Key + RecordID): 12 bytes.
Max Capacity: 4068 / 12 = 339 items.
*/


/* eg- 
+---------------------------------------+
| Header                                |
| Number of entries = 4                 |
| NextLeafPointer                       |
|---------------------------------------|
| Key=10  -> RID(Page100, Slot1)        |
| Key=15  -> RID(Page120, Slot5)        |
| Key=18  -> RID(Page200, Slot2)        |
| Key=20  -> RID(Page340, Slot8)        |
+---------------------------------------+
*/


class BPlusTreeLeafPage : public BPlusTreePage {
private:
    int next_page_id_;
    MappingType array_[339];

public:
    // Initialization
    void Init(int page_id, int parent_id = -1, int max_size = 339);

    // Getters / Setters for next_page_id_
    int GetNextPageId() const;
    void SetNextPageId(int next_page_id);

    // Array access
    int KeyAt(int index) const;
    RecordID ValueAt(int index) const;
    
    // Core Engine Logic (The Algorithm)
    int Insert(int key, const RecordID& value);

    bool Lookup(int key, RecordID* result) const;
    void MoveHalfTo(BPlusTreeLeafPage* recipient);
};

#endif