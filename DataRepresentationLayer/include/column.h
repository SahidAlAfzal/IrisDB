#ifndef COLUMN_H
#define COLUMN_H

#include <cstdint>
#include <string>

#include "value.h"

class Column {
private:
    std::string column_name_;
    TypeId column_type_;
    uint32_t column_length_;
    uint32_t column_offset_;

public:
    Column(std::string name, TypeId type, uint32_t length);

    std::string GetName() const;
    TypeId GetType() const;
    uint32_t GetLength() const;
    uint32_t GetOffset() const;

    // Only the Schema class should call this when calculating tuple layouts
    void SetOffset(uint32_t offset);
};

#endif // COLUMN_H