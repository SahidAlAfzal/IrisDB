#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "b_plus_tree_page.h"
#include "b_plus_leaf_page.h"
#include "b_plus_internal_page.h"
#include "../Buffer/BufferPoolManager.h"


class BPlusTree {
private:
    int root_page_id_;
    BufferPoolManager* bpm_;

    // Helper: Traverses the tree to find the correct leaf node for a given key.
    BPlusTreeLeafPage* FindLeafPage(int key);
    
    // Helper: Creates a brand new tree if it's completely empty.
    void StartNewTree(int key, const RecordID& value);

    // Helper: Handles pushing a new split key up into the parent internal node.
    void InsertIntoParent(BPlusTreePage* old_node, int key, BPlusTreePage* new_node);

public:
    BPlusTree(BufferPoolManager* bpm);

    bool Insert(int key, const RecordID& value);

    bool GetValue(int key, RecordID* result);
};

#endif