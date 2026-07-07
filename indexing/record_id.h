#ifndef RECORD_ID_H
#define RECORD_ID_H

struct RecordID {
    int page_id_;
    int slot_id_;

    RecordID(){
        page_id_ = slot_id_ = -1;
    }

    RecordID(int page_id, int slot_id) : page_id_(page_id), slot_id_(slot_id) {};

    bool operator==(const RecordID& other){
        return page_id_ == other.page_id_ && slot_id_ == other.slot_id_;
    }
};

#endif