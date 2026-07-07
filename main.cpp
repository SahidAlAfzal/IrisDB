#include <iostream>
#include <string>
#include <sstream>
#include "Buffer/BufferPoolManager.h"
#include "indexing/b_plus_tree.h"
#include "indexing/record_id.h"
#include "Page/slotted_page.h"

int main() {
    std::cout << "=======================================" << std::endl;
    std::cout << "        Welcome to IrisDB CLI          " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Commands: " << std::endl;
    std::cout << "  INSERT <id> <string_data> (e.g., INSERT 5000 Alice)" << std::endl;
    std::cout << "  SELECT <id>               (e.g., SELECT 5000)" << std::endl;
    std::cout << "  EXIT" << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    BufferPoolManager* bpm = new BufferPoolManager();
    BPlusTree* tree = new BPlusTree(bpm);

    int current_data_page_id = -1;
    SlottedPage* current_data_page = nullptr;

    std::string line;
    while (true) {
        std::cout << "irisdb> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit" || line == "EXIT") break;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "insert" || command == "INSERT") {
            int id;
            std::string data;
            if (!(ss >> id >> data)) {
                std::cout << "Error: Format is INSERT <id> <data>" << std::endl;
                continue;
            }

            // 1. Ask the Buffer Pool for a page and wrap it properly!
            if (current_data_page_id == -1) {
                Page* raw_page = bpm->NewPage(&current_data_page_id);
                current_data_page = new SlottedPage(raw_page->data); // <--- THE FIX
                current_data_page->Init(current_data_page_id);
            }

            // 2. Try to insert the string
            bool success = current_data_page->InsertTuple(data.c_str(), data.length() + 1);

            // 3. If the page is full, save it, and get a brand new one
            if (!success) {
                bpm->UnpinPage(current_data_page_id, true);
                Page* raw_page = bpm->NewPage(&current_data_page_id);
                
                delete current_data_page; // Clean up old wrapper
                current_data_page = new SlottedPage(raw_page->data); // <--- THE FIX
                
                current_data_page->Init(current_data_page_id);
                current_data_page->InsertTuple(data.c_str(), data.length() + 1);
            }

            // 4. Get the slot ID and insert into B+ Tree
            int slot_id = current_data_page->GetHeader()->num_of_tuples - 1;
            tree->Insert(id, RecordID(current_data_page_id, slot_id));
            std::cout << "Success: Inserted 1 row." << std::endl;

        } 
        else if (command == "select" || command == "SELECT") {
            int id;
            if (!(ss >> id)) {
                std::cout << "Error: Format is SELECT <id>" << std::endl;
                continue;
            }

            RecordID rid;
            if (tree->GetValue(id, &rid)) {
                // Wrap the fetched raw memory on the stack 
                Page* raw_page = bpm->FetchPage(rid.page_id_);
                SlottedPage data_page(raw_page->data); // <--- THE FIX

                char* tuple_data = data_page.getTuple(rid.slot_id_);
                
                if (tuple_data != nullptr) {
                    std::cout << "FOUND -> ID: " << id << " | DATA: " << tuple_data << std::endl;
                } else {
                    std::cout << "Data was deleted." << std::endl;
                }

                bpm->UnpinPage(rid.page_id_, false);
            } else {
                std::cout << "Record not found." << std::endl;
            }
        } 
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    if (current_data_page_id != -1) {
        bpm->UnpinPage(current_data_page_id, true);
        delete current_data_page; // Clean up memory
    }
    
    std::cout << "Shutting down IrisDB..." << std::endl;
    delete tree;
    delete bpm;
    return 0;
}