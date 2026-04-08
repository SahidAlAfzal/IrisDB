#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include<fstream>
#include<string>

class DiskManager {
private:
    std::fstream db_file;
    std::string filename;
    int num_pages;
    


public:
    static const int PAGE_SIZE = 4096;  // 4KB

    DiskManager(const std::string &filename);
    ~DiskManager();


    void WritePage(int page_id, const char* page_data);
    void ReadPage(int page_id, char* page_data);
    int GetFileSize();

};
#endif