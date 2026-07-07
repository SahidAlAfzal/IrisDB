#include<iostream>
#include "BufferPoolManager.h"


BufferPoolManager::BufferPoolManager(){
    disk_manager = new DiskManager("test.db");
    for(int i=0;i<POOL_SIZE;i++){
        free_frames.push_back(i);
    }
}

BufferPoolManager::~BufferPoolManager() {
    delete disk_manager;
}


Page* BufferPoolManager::FetchPage(int page_id){
    //Cache hit
    if(page_table.find(page_id) != page_table.end()){    // page is in page_table
        int frame_id = page_table[page_id];
        frames[frame_id].pin_count++;
        replacer.Pin(frame_id);
        return (frames + frame_id);

    } 

    // Cache Miss
    else {
        // page is NOT in page_table
        
        if(free_frames.empty()){
            // Replacement is needed.
            int victim_id;
            if(replacer.Victim(&victim_id)){     // if false means free frame is not empty.

                //before you erase the old frame during replacement, check if it is dirty or not
                if(frames[victim_id].is_dirty){
                    disk_manager->WritePage(
                        frames[victim_id].page_id,
                        frames[victim_id].data
                    );
                }

                //unregister in page_table
                page_table.erase(frames[victim_id].page_id);

                disk_manager->ReadPage(
                    page_id,
                    frames[victim_id].data
                );

                frames[victim_id].page_id = page_id;
                frames[victim_id].is_dirty = false;
                // register in page_table
                page_table[page_id] = victim_id;

                frames[victim_id].pin_count = 1;

                //replacer.Pin(victim_id);     // pin the new frame
                return (frames + victim_id);
            } else {
                /* 
                free frame is empty -> no frame is available so replacement But no frame in dll,
                nobody is for eviction, eat 5 stars now
                */
                return nullptr;
            }

        } else {
            int free_frame_id = free_frames.front();
            free_frames.pop_front();

            disk_manager->ReadPage(page_id,frames[free_frame_id].data);       // Reading from disk
            frames[free_frame_id].page_id = page_id;

            page_table[page_id] = free_frame_id;   // registering in page table

            frames[free_frame_id].pin_count = 1;

            //replacer.Pin(free_frame_id);           // Pinning the frame
            return (frames + free_frame_id);
        }
    }
}




// UnpinPage() API is responsible for putting them into the LRU
bool BufferPoolManager::UnpinPage(int page_id, bool is_dirty){
    // if it is pinned, so not in dll but in page table
    if(page_table.find(page_id) == page_table.end()){
        return false;
    }

    int frame_id = page_table[page_id];

    if(is_dirty){
        frames[frame_id].is_dirty = true;
    }

    frames[frame_id].pin_count--;

    if(frames[frame_id].pin_count == 0){     // if no thread is pinning the page rn 
        replacer.Unpin(frame_id);
        return true;
    }

    return false;
}





Page* BufferPoolManager::NewPage(int* page_id){
    // *page_id = disk_manager->AllocatePage(); --> will lead to "Page Leak" => 1.if2.else return nullptr that's means no frame is empty + no one available for eviction, no new page but we already incremented num_pages.
    int frame_id;

    if(free_frames.empty()){
        // Replacement is needed.
        if(replacer.Victim(&frame_id)){     // if false means free frame is not empty.

            //before you erase the old frame during replacement, check if it is dirty or not
            if(frames[frame_id].is_dirty){
                disk_manager->WritePage(
                    frames[frame_id].page_id,
                    frames[frame_id].data
                );
            }

            //unregister in page_table
            page_table.erase(frames[frame_id].page_id);

            
        } else {
            /* 
            free frame is empty -> no frame is available so replacement But no frame in dll,
            nobody is for eviction, eat 5 stars now
            */
            return nullptr;
        }

    } else {
        frame_id = free_frames.front();
        free_frames.pop_front();
    }

    *page_id = disk_manager->AllocatePage();
    page_table[*page_id] = frame_id;
    frames[frame_id].page_id = *page_id;
    frames[frame_id].pin_count = 1;
    frames[frame_id].is_dirty = true; // IMPORTANT: Mark it dirty so it eventually gets saved to the .db file


    // Wipe the old data with new data
    for(int i=0;i<PAGE_SIZE;i++){
        frames[frame_id].data[i] = 0;
    }


    // Return the pointer
    return (frames + frame_id);
}