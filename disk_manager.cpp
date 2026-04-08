#include "disk_manager.h"
#include<fstream>
#include<string>
#include<iostream>


DiskManager::DiskManager(const std::string& filename) {
    this->filename = filename;
    num_pages = 0;

    //if doesn't exist => create it!
    if(!std::ifstream(filename,std::ios::in).good()) {
        std::ofstream(filename,std::ios::out);  //created it
    }

    db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // determine file size → number of pages
    db_file.seekg(0,std::ios::end);
    int total_bytes = db_file.tellg();      // total no. of bytes in db file
    num_pages = total_bytes / PAGE_SIZE;
}

DiskManager::~DiskManager() {
    if(db_file.is_open()) {
        db_file.close();
    }
}

void DiskManager::ReadPage(int page_id, char* page_data){
    int offset = page_id * PAGE_SIZE;     // we need offset to move the pointer to correct place

    db_file.seekg(offset, std::ios::beg);
    db_file.read(page_data,PAGE_SIZE);   // page_data points to an empty array


    //-------------------------------------------- Prevent FailBit
    if(db_file.fail()){
        db_file.clear(); // Resets the stream state so future writes don't fail
        std::cout << "This page doesn't exist yet!" << std::endl;
    }
    //--------------------------------------------
}


void DiskManager::WritePage(int page_id, const char* page_data) {
    int offset = page_id * PAGE_SIZE;

    db_file.seekp(offset, std::ios::beg);
    db_file.write(page_data,PAGE_SIZE);
    db_file.flush();     // rather than putting data in in-memory buffer first, directly write the data in the db_file itself
    
    if(page_id >= num_pages) {num_pages++;}
}


int DiskManager::GetFileSize() {
    return num_pages * PAGE_SIZE;
}


