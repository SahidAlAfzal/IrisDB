#ifndef LRU_REPLACER_H
#define LRU_REPLACER_H

#include<unordered_map>
#include "Node.h"

class LRUReplacer {
public:
    Node* head;
    Node* tail;
    std::unordered_map<int, Node*> mp;    // {frame_id, pointer to the page}
    int capacity,curr_size;

private:
    void deleteNode(Node* temp);
    void putNode(Node* temp);

public:
    LRUReplacer(int capacity);
    bool Victim(int* fid);
    void Pin(int fid);
    void Unpin(int fid);
    int Size();
    ~LRUReplacer();
};

#endif
