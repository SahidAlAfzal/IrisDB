#include "b_plus_tree_page.h"


IndexPageType BPlusTreePage::GetPageType() const{
    return page_type_;
}

void BPlusTreePage::SetPageType(IndexPageType page_type){
    this->page_type_ = page_type;
}

int BPlusTreePage::GetSize() const{
    return size_;
}

void BPlusTreePage::SetSize(int size){
    this->size_ = size;
}

void BPlusTreePage::IncreaseSize(int amount){
    this->size_ += amount;
}

int BPlusTreePage::GetMaxSize() const{
    return max_size_;
}

void BPlusTreePage::SetMaxSize(int max_size){
    this->max_size_ = max_size;
}

int BPlusTreePage::GetParentPageId() const{
    return parent_page_id_;
}

void BPlusTreePage::SetParentPageId(int parent_page_id){
    this->parent_page_id_ = parent_page_id;
}

int BPlusTreePage::GetPageId() const{
    return page_id_;
}

void BPlusTreePage::SetPageId(int page_id){
    this->page_id_ = page_id;
}