#ifndef B_PLUS_TREE_INTERNAL_PAGE_H
#define B_PLUS_TREE_INTERNAL_PAGE_H
#include "b_plus_tree_page.h"
#include<utility>

#define InternalMappingType std::pair<int,int>   // {key, pointer(page_id)}
/*
+----------------------------------+
| Header                           |
| Number of keys = 3               |
| IsLeaf = false                   |
|----------------------------------|
| ChildPtr0                        |
| Key1 = 20                        |
| ChildPtr1                        |
| Key2 = 50                        |
| ChildPtr2                        |
| Key3 = 80                        |
| ChildPtr3                        |
+----------------------------------+
*/

class BPlusTreeInternalPage : public BPlusTreePage {
private:
    // 509 * 8 bytes = 4072 bytes. Total: 24 (header) + 4072 = 4096
    

public:
    InternalMappingType array_[509];
    // Initialization
    void Init(int page_id, int parent_id = -1, int max_size = 509);

    // Array access
    int KeyAt(int index) const;
    void SetKeyAt(int index, int key);
    int ValueAt(int index) const;
    void SetValueAt(int index, int next_node_page_id);
    int GetSize() const;
    void SetSize(int size);
    void IncreaseSize(int amount);

    // Routing Logic
    int Lookup(int key) const;
    int Insert(int key, const int value);
    void MoveHalfTo(BPlusTreeInternalPage* recipient);

};

#endif