#include "b_plus_leaf_page.h"
#include "../Buffer/BufferPoolManager.h"

/*
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


// Initialization
void BPlusTreeLeafPage::Init(int page_id, int parent_id, int max_size){
    SetPageType(IndexPageType::LEAF_PAGE);
    SetPageId(page_id);
    SetMaxSize(max_size);
    SetNextPageId(-1);
    SetParentPageId(parent_id);
    SetSize(0);
}



// Getters / Setters for next_page_id_
int BPlusTreeLeafPage::GetNextPageId() const{
    return next_page_id_;
}


void BPlusTreeLeafPage::SetNextPageId(int next_page_id){
    next_page_id_ = next_page_id;
}

// Array access
int BPlusTreeLeafPage::KeyAt(int index) const{
    return array_[index].first;
}

RecordID BPlusTreeLeafPage::ValueAt(int index) const{
    return array_[index].second;
}



int BPlusTreeLeafPage::Insert(int key, const RecordID& value) {
    int n = GetSize();
    
    // We will handle splitting at the Tree level later!
    if(n == GetMaxSize()){
        return n; 
    }

    // 1. Find insertion index
    int target_index = 0;
    int left = 0;
    int right = n; // Note: n, not n-1. We might insert at the very end!

    while(left < right){
        int mid = left + (right - left) / 2;
        if(array_[mid].first < key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    target_index = left; // 'left' is now the exact slot we need

    // 2. Shift everything to the right by 1
    // Notice we start from 'n' and pull data from 'i-1'
    for(int i = n; i > target_index; i--){
        array_[i] = array_[i - 1];
    }
    
    // 3. Insert and update size
    array_[target_index] = {key, value};
    IncreaseSize(1);
    
    return GetSize();
}



bool BPlusTreeLeafPage::Lookup(int key, RecordID* result) const{
    if(GetSize() == 0) return false;

    int n = GetSize();
    int l=0,r=n-1;

    while(l <= r){
        int mid = l + (r-l)/2;
        if(array_[mid].first < key) l = mid+1;
        else if(key < array_[mid].first) r = mid-1;
        else {
            *result = array_[mid].second;
            return true;
        }
    }

    return false;
}


void BPlusTreeLeafPage::MoveHalfTo(BPlusTreeLeafPage* recipient) {
    int split_index = GetMaxSize() / 2;
    int move_count = GetSize() - split_index;

    // 1. Copy the data over. 
    // Notice how we map index `split_index + i` from the old array 
    // to index `i` of the completely empty recipient array!
    for (int i = 0; i < move_count; i++) {
        recipient->array_[i] = array_[split_index + i];
    }

    // 2. Update the Linked List for fast scans
    recipient->SetNextPageId(GetNextPageId());
    SetNextPageId(recipient->GetPageId());

    // 3. Update the sizes!
    recipient->SetSize(move_count);
    SetSize(split_index); 
}