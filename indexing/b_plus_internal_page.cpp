#include "b_plus_internal_page.h"


void BPlusTreeInternalPage::Init(int page_id, int parent_id, int max_size){
    SetPageType(IndexPageType::INTERNAL_PAGE);
    SetPageId(page_id);
    SetMaxSize(max_size);
    SetParentPageId(parent_id);
    SetSize(0);
}

void BPlusTreeInternalPage::IncreaseSize(int amount) {
    if(GetSize() + amount <= GetMaxSize())
        SetSize(GetSize() + amount);
}

int BPlusTreeInternalPage::KeyAt(int index) const{
    if(index < 0 || index >= GetSize()) return -1;
    return array_[index].first;
}

void BPlusTreeInternalPage::SetKeyAt(int index, int key){
    if(index < 0 || index >= GetSize()) return;

    array_[index].first = key;
}

int BPlusTreeInternalPage::ValueAt(int index) const {
    if(index < 0 || index >= GetSize()) return -1;

    return array_[index].second;
}

void BPlusTreeInternalPage::SetValueAt(int index,const int Value){
    if(index < 0 || index >= GetSize()) return;

    array_[index].second = Value; // Page_id pointing to
}

int BPlusTreeInternalPage::GetSize() const{
    return BPlusTreePage::GetSize();
}

void BPlusTreeInternalPage::SetSize(int size){
    BPlusTreePage::SetSize(size);
}


// Routing Logic
int BPlusTreeInternalPage::Lookup(int key) const {
    // [INVALID, 10, 20, 30] and their values are [P0, P1, P2, P3]
    // [<GARBAGE, P0>, <10, P1>, <20, P2>, <30, P3>]

    int l=1, r=GetSize();

    while(l < r){
        int mid = l + (r-l)/2;

        if(array_[mid].first <= key) l = mid+1;
        else r = mid;
    }

    return array_[l-1].second;   // page id
}


int BPlusTreeInternalPage::Insert(int key, const int value){    // value = page_id (pointer)
    int n = GetSize();
    
    // We will handle splitting at the Tree level later!
    if(n == GetMaxSize()){
        return n;
    }

    // 1. Find insertion index
    int target_index = 0;
    int left = 1;
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



void BPlusTreeInternalPage::MoveHalfTo(BPlusTreeInternalPage* recipient){
    int split_index = GetMaxSize() / 2;
    int move_count = GetSize() - split_index;

    // 1. Copy the data over. 
    // Notice how we map index `split_index + i` from the old array 
    // to index `i` of the completely empty recipient array!
    for (int i = 0; i < move_count; i++) {
        recipient->array_[i] = array_[split_index + i];
    }

    // 2. Update the sizes!
    recipient->SetSize(move_count);
    SetSize(split_index);
}