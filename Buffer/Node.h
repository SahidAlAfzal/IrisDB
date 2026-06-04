#pragma once
class Node {
public:
    int frame_id;
    Node* prev,*next;
    Node(int id);
};