#include<iostream>
#include "Node.h"

Node::Node(int fid){
    frame_id = fid;
    prev = next = nullptr;
}