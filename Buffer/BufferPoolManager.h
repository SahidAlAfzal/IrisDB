#ifndef BUFFER_POOL_MANAGER_H
#define BUFFER_POOL_MANAGER_H

#include<iostream>
#include<list>
#include<vector>
#include<unordered_map>
#include "LRUReplacer.h"
#include "Page.h"
#include "../disk_manager.h"

const int POOL_SIZE = 100;
const int PAGE_SIZE = 4096;

class BufferPoolManager {
    Page frames[POOL_SIZE];
    std::unordered_map<int,int> page_table;    // page_id -> frame_id
    std::list<int> free_frames;
    LRUReplacer replacer = LRUReplacer(100);
    DiskManager* disk_manager;

public:
    BufferPoolManager();
    Page* FetchPage(int page_id);

    // UnpinPage() API is responsible for putting them into the LRU
    bool UnpinPage(int page_id, bool is_dirty);
    Page* NewPage(int* page_id);
};

#endif