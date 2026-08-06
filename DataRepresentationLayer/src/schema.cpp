#include "include/schema.h"

Schema::Schema(const std::vector<Column>& columns, uint32_t tuple_length) {
    columns_ = columns;
    tuple_length_ = tuple_length;

    uint32_t curr_offset = 0;
    for (auto& col : columns_) {
        col.SetOffset(curr_offset);
        curr_offset += col.GetLength();
    }

    tuple_length_ = curr_offset;
}

const std::vector<Column>& Schema::GetColumns() const {
    return columns_;
}

const Column& Schema::GetColumn(uint32_t col_idx) const {
    return columns_[col_idx];
}

uint32_t Schema::GetTupleLength() const {
    return tuple_length_;
}