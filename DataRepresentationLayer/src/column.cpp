#include "include/column.h"

Column::Column(std::string name, TypeId type, uint32_t length) {
    column_name_ = name;
    column_type_ = type;
    column_length_ = length;
    column_offset_ = 0;
}

std::string Column::GetName() const {
    return column_name_;
}

TypeId Column::GetType() const {
    return column_type_;
}

uint32_t Column::GetLength() const {
    return column_length_;
}

uint32_t Column::GetOffset() const {
    return column_offset_;
}

void Column::SetOffset(uint32_t offset) {
    column_offset_ = offset;
}