#ifndef SCHEMA_H
#define SCHEMA_H

#include <cstdint>
#include <vector>

#include "column.h"

// The Schema is simply an ordered collection of Column objects.
class Schema {
private:
    std::vector<Column> columns_;
    uint32_t tuple_length_;

public:
    Schema(const std::vector<Column>& columns, uint32_t tuple_length = 0);

    const std::vector<Column>& GetColumns() const;
    const Column& GetColumn(uint32_t col_idx) const;
    uint32_t GetTupleLength() const;
};

#endif // SCHEMA_H