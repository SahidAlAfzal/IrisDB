#include "Page.h"

Page::Page(){
    page_id = -1;
    pin_count = 0;
    is_dirty = false;
}


Page::Page(int page_id, char* page_data){
    this->page_id = page_id;
    pin_count = 0;
    is_dirty = false;
    for(int i=0;i<PAGE_SIZE;i++){
        data[i] = page_data[i];
    }
}