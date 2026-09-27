#ifndef TUPLE_H
#define TUPLE_H

#include <vector>
#include <cstdint>
#include "schema.h"
#include "value.h"

// A Tuple represents a single row (or record) of data.
// It is responsible for taking a high-level representation (vector of Values)
// and serializing it into a low-level byte array (data_) based on a Schema.
// It also handles deserialization: pulling a Value back out from the byte array.
class Tuple {
private:
    uint32_t size_;
    char* data_;
    bool allocated_; // True if we dynamically allocated data_ and own it

public:
    // 1. Serialization Constructor
    // Creates a Tuple from a list of values and a schema.
    // It allocates memory and packs the values into the byte array.
    Tuple(const std::vector<Value>& values, const Schema* schema);

    // 2. Deserialization Constructor
    // Wraps an existing byte array (e.g., from a SlottedPage) into a Tuple.
    // Memory is NOT owned by this Tuple (no allocation/deletion).
    Tuple(const char* data, uint32_t size);

    // Default constructor for an empty tuple
    Tuple();

    // Destructor
    ~Tuple();

    // Copy Constructor & Assignment Operator
    Tuple(const Tuple& other);
    Tuple& operator=(const Tuple& other);

    // Get a specific value from the tuple based on the schema column index
    Value GetValue(const Schema* schema, uint32_t column_idx) const;

    // Get the underlying raw byte array
    const char* GetData() const;

    // Get the length of the serialized data
    uint32_t GetLength() const;
};

#endif // TUPLE_H
