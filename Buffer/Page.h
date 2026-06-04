#ifndef PAGE_H
#define PAGE_H

const int PAGE_SIZE = 4096;

class Page {
public:
    int page_id;
    int pin_count;
    bool is_dirty;

    char data[PAGE_SIZE];

    Page();
    Page(int page_id, char* page_data);
};

#endif