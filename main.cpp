#include<iostream>
#include "disk_manager.h"

int main() {
    DiskManager dm("test.db");

    char page_data[DiskManager::PAGE_SIZE] = "Hello, World!";
    dm.WritePage(0, page_data);

    char read_data[DiskManager::PAGE_SIZE];
    dm.ReadPage(1, read_data);

    std::cout << "Read from disk: " << read_data << std::endl;

    return 0;
}