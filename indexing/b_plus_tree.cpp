#include "b_plus_tree.h"

// class BPlusTree {
// private:
//     int root_page_id_;
//     BufferPoolManager* bpm_;

//     // Helper: Traverses the tree to find the correct leaf node for a given key.
//     BPlusTreeLeafPage* FindLeafPage(int key);
    
//     // Helper: Creates a brand new tree if it's completely empty.
//     void StartNewTree(int key, const RecordID& value);

//     // Helper: Handles pushing a new split key up into the parent internal node.
//     void InsertIntoParent(BPlusTreePage* old_node, int key, BPlusTreePage* new_node);

// public:
//     BPlusTree(BufferPoolManager* bpm);

//     bool Insert(int key, const RecordID& value);

//     bool GetValue(int key, RecordID* result);
// };

// #endif

BPlusTreeLeafPage* BPlusTree::FindLeafPage(int key) {
    /* Most Important: You must unpin the parent before (or immediately after) you fetch the child. 
    This is known in database engineering as "Hand-over-Hand" or "Crab" traversal.
    */

    // Safety check
    if(root_page_id_ == -1){
        return nullptr;
    }

    int curr_page_id = root_page_id_;
    Page* raw_page = bpm_->FetchPage(curr_page_id);
    BPlusTreePage* curr_node = reinterpret_cast<BPlusTreePage*> (raw_page->data);

    while(curr_node->GetPageType() == IndexPageType::INTERNAL_PAGE){
        
        BPlusTreeInternalPage* internal_node = reinterpret_cast<BPlusTreeInternalPage*> (curr_node);
        int child_node_id = internal_node->Lookup(key);

        // Before diving into the next child node, unpin current one
        bpm_->UnpinPage(curr_node->GetPageId(), false);    // traversal -> no modification -> not dirty

        curr_page_id = child_node_id;
        Page* child_page = bpm_->FetchPage(child_node_id);
        curr_node = reinterpret_cast<BPlusTreePage*> (child_page->data);
    }

    // Gets out => curr_node is Leaf node
    return reinterpret_cast<BPlusTreeLeafPage*> (curr_node);
}


void BPlusTree::StartNewTree(int key, const RecordID& value){
    // no tree, so lets ask for a page for root node of b+ tree
    Page* raw_page = bpm_->NewPage(&root_page_id_);

    if(raw_page == nullptr) {
        std::cout << "Buffer Pool is completely full" << std::endl;
        return;
    }

    BPlusTreeLeafPage* root_node = reinterpret_cast<BPlusTreeLeafPage*> (raw_page->data);
    root_node->Init(root_page_id_);   // -1, 339 (default)
    root_node->Insert(key, value);

    // The Buffer Pool Rule! (Mark as dirty because we inserted data)
    bpm_->UnpinPage(root_page_id_, true);
}


void BPlusTree::InsertIntoParent(BPlusTreePage* old_node, int key, BPlusTreePage* new_node){
    // Scenario A: old_node was the root node
    if(old_node->GetPageId() == root_page_id_){
        
        Page* raw_page = bpm_->NewPage(&root_page_id_);
        BPlusTreeInternalPage* new_root_node = reinterpret_cast<BPlusTreeInternalPage*> (raw_page->data);
        new_root_node->Init(root_page_id_,-1,509);

        // two leaf node's parent is root node
        old_node->SetParentPageId(root_page_id_);
        new_node->SetParentPageId(root_page_id_);


        // [INVALID , Middle_key] [P0, P1] where P0 old_node_page_id and P1 new_node_page_id
        new_root_node->SetSize(2);
        int GARBAGE = 0;
        new_root_node->SetKeyAt(0,GARBAGE);
        new_root_node->SetValueAt(0,old_node->GetPageId());
        new_root_node->SetKeyAt(1,key);
        new_root_node->SetValueAt(1,new_node->GetPageId());

        new_root_node->SetSize(2);

        // Unpin the new_root_node & page taken for the new root is dirty
        bpm_->UnpinPage(root_page_id_, true);
    }

    // Scenario B: The Old Node has a Parent
    else {
        int parent_id = old_node->GetParentPageId();
        Page* raw_parent = bpm_->FetchPage(parent_id);
        BPlusTreeInternalPage* parent_node = reinterpret_cast<BPlusTreeInternalPage*> (raw_parent->data);


        // Scenario B-1: The Parent has room!
        if(parent_node->GetSize() < parent_node->GetMaxSize()){
            parent_node->Insert(key, new_node->GetPageId());
            bpm_->UnpinPage(parent_id, true);  // dirty
            return;
        } 

        // Scenario B-2: The Parent is FULL. We must split and recurse!
        else {
            int new_internal_id;
            Page* raw_page = bpm_->NewPage(&new_internal_id);
            BPlusTreeInternalPage* new_internal_node = reinterpret_cast<BPlusTreeInternalPage*> (raw_page->data);
            new_internal_node->Init(new_internal_id, parent_node->GetParentPageId(), parent_node->GetMaxSize());

            
            // half of parent node content shifting to new_internal_node
            parent_node->MoveHalfTo(new_internal_node);

            // insert the key now in the internal node
            int new_middle_key = new_internal_node->KeyAt(0);
            if(new_middle_key <= key){
                new_internal_node->Insert(key, new_node->GetPageId());
            } else {
                parent_node->Insert(key, new_node->GetPageId());
            }

            // -----------------------------------------------------------------------------------------
            // Half child node is treating parent_node as parent where it should new_internal_node
            for (int i = 0; i < new_internal_node->GetSize(); i++) {
                int child_page_id = new_internal_node->ValueAt(i);
                
                // Fetch the child from the buffer pool
                Page* raw_child = bpm_->FetchPage(child_page_id);
                BPlusTreePage* child_node = reinterpret_cast<BPlusTreePage*>(raw_child->data);
                
                // Tell the child who its new parent is
                child_node->SetParentPageId(new_internal_node->GetPageId());
                
                // Unpin it as dirty!
                bpm_->UnpinPage(child_page_id, true);
            }
            // -----------------------------------------------------------------------------------------

            // Recursively middle node goes to upper internal nodes / empty (in that case new root node)
            InsertIntoParent(parent_node, new_middle_key, new_internal_node);


            // Unpin the parent_node & new_internal_node
            bpm_->UnpinPage(parent_id, true);
            bpm_->UnpinPage(new_internal_id, true);
        }
    }



}

BPlusTree::BPlusTree(BufferPoolManager* bpm){
    root_page_id_ = -1;
    bpm_ = bpm;
}

bool BPlusTree::Insert(int key, const RecordID& value){
    if(root_page_id_ == -1) {
        StartNewTree(key, value);
        return true;
    }

    BPlusTreeLeafPage* leaf = FindLeafPage(key);
    if(leaf->GetSize() != leaf->GetMaxSize()) {
        leaf->Insert(key, value);
        bpm_->UnpinPage(leaf->GetPageId(), true); 
        return true;
    }

    else {     // Node is full, splitting
        int new_page_id;
        Page* raw_page = bpm_->NewPage(&new_page_id);

        // Safety check: If Buffer Pool is completely full
        if (raw_page == nullptr) return false;

        // This Page is for our new leaf node
        BPlusTreeLeafPage* new_leaf = reinterpret_cast<BPlusTreeLeafPage*>(raw_page->data);
        new_leaf->Init(new_page_id,leaf->GetParentPageId(),leaf->GetMaxSize());

        leaf->MoveHalfTo(new_leaf);
        int middle_key = new_leaf->KeyAt(0);

        // key belongs to which node
        if(key < middle_key) {
            leaf->Insert(key, value);
        } else if(middle_key <= key){
            new_leaf->Insert(key, value);
        }

        // Push the middle key up the tree
        InsertIntoParent(leaf, middle_key, new_leaf);


        // Unpin both & both pages are dirty
        bpm_->UnpinPage(leaf->GetPageId(), true);
        bpm_->UnpinPage(new_leaf->GetPageId(), true);

        return true;
    }
}


bool BPlusTree::GetValue(int key, RecordID* result){
    if(root_page_id_ == -1) return false;

    BPlusTreeLeafPage* leaf_node = FindLeafPage(key);
    bool find = leaf_node->Lookup(key,result);

    // Unpin
    bpm_->UnpinPage(leaf_node->GetPageId(), false);
    return find;
}

