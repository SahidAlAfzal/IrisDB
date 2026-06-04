#include<iostream>
#include "LRUReplacer.h"
#include<unordered_map>

void LRUReplacer::deleteNode(Node* temp){
    Node* prevNode = temp->prev;
    Node* nextNode = temp->next;

    prevNode->next = nextNode;
    nextNode->prev = prevNode;
}

void LRUReplacer::putNode(Node* temp){
    Node* adjNode = head->next;
    head->next = temp;
    temp->prev = head;

    temp->next = adjNode;
    adjNode->prev = temp;
}

LRUReplacer::LRUReplacer(int capacity) {
    curr_size = 0;
    head = new Node(-1);
    tail = new Node(-1);
    this->capacity = capacity;
    head->next = tail;
    tail->prev = head;
}

bool LRUReplacer::Victim(int* fid){
    if(curr_size == 0) return false;
    Node* victim = tail->prev;
    *fid = victim->frame_id;

    mp.erase(*fid);
    deleteNode(victim);
    delete victim;


    curr_size--;
    return true;
}
    
    
void LRUReplacer::Pin(int fid){
    if(mp.find(fid) == mp.end()) return; // Already pinned!
    Node* pinned_node = mp[fid];
    mp.erase(fid);
    deleteNode(pinned_node);
    delete pinned_node;

    curr_size--;
    // std::cout << fid << "is pinned!" << std::endl;
}

void LRUReplacer::Unpin(int fid){
    if(mp.find(fid) != mp.end()) return;  // already unpinned
    if(curr_size >= capacity) {
        //std::cout << "Maximum size is reached!" << std::endl;
        return;
    }

    Node* new_node = new Node(fid);
    mp[fid] = new_node;
    putNode(new_node);               // Most recently used frame
    curr_size++;
    // std::cout << fid << "is unpinned!" << std::endl;
}

int LRUReplacer::Size(){
    //returns the number of frame eligible for eviction
    return this->curr_size;
}


LRUReplacer::~LRUReplacer() {
    Node* cur = head;
    while(cur){
        Node* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
}
