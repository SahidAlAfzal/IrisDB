#include "../include/tuple.h"
#include <cstring>
#include <stdexcept>

// Default constructor
Tuple::Tuple() : size_(0), data_(nullptr), allocated_(false) {}

// Serialization Constructor
Tuple::Tuple(const std::vector<Value> &values, const Schema *schema) {
  if (values.size() != schema->GetColumns().size()) {
    throw std::invalid_argument(
        "Number of values does not match schema columns.");
  }

  size_ = schema->GetTupleLength();
  data_ = new char[size_];
  allocated_ = true;

  // Initialize memory to zero to avoid garbage bytes in padding
  std::memset(data_, 0, size_);

  // Iterate through schema and pack values into data_
  for (size_t i = 0; i < schema->GetColumns().size(); i++) {
    const Column &col = schema->GetColumn(i);
    const Value &val = values[i];

    // Ensure the value type matches the column type
    if (val.getType() != col.GetType() && !val.isNull()) {
      throw std::invalid_argument("Value type mismatch at column " +
                                  std::to_string(i));
    }

    uint32_t offset = col.GetOffset();

    if (val.isNull()) {
      // If it's null, we just leave it zeroed out for now.
      // A more advanced engine would use a null bitmap header.
      continue;
    }

    // Serialize based on type
    switch (col.GetType()) {
    case TypeId::INTEGER: {
      int32_t int_val = val.getAsInt();
      std::memcpy(data_ + offset, &int_val, sizeof(int32_t));
      break;
    }
    case TypeId::BOOLEAN: {
      bool bool_val = val.getAsBool();
      std::memcpy(data_ + offset, &bool_val, sizeof(bool));
      break;
    }
    case TypeId::VARCHAR: {
      std::string str_val = val.getAsString();
      // Ensure we don't overflow the column length
      uint32_t copy_len =
          std::min(static_cast<uint32_t>(str_val.length()), col.GetLength());
      std::memcpy(data_ + offset, str_val.c_str(), copy_len);
      // Null termination is implicitly handled since we memset the whole array
      // to 0 initially
      break;
    }
    default:
      throw std::logic_error("Unsupported type for serialization.");
    }
  }
}

// Deserialization Constructor (Wraps existing memory)
Tuple::Tuple(const char *data, uint32_t size) {
  size_ = size;
  // We do NOT copy the data, we just point to it.
  // This is crucial for performance when reading from SlottedPages.
  data_ = const_cast<char *>(data);
  allocated_ = false;
}

// Destructor
Tuple::~Tuple() {
  if (allocated_ && data_ != nullptr) {
    delete[] data_;
  }
}

// Copy Constructor (Deep copy)
Tuple::Tuple(const Tuple &other) {
  size_ = other.size_;
  allocated_ = other.allocated_;
  if (other.allocated_ && other.data_ != nullptr) {
    data_ = new char[size_];
    std::memcpy(data_, other.data_, size_);
  } else {
    data_ = other.data_;
  }
}

// Assignment Operator (Deep copy)
Tuple &Tuple::operator=(const Tuple &other) {
  if (this == &other)
    return *this;

  if (allocated_ && data_ != nullptr) {
    delete[] data_;
  }

  size_ = other.size_;
  allocated_ = other.allocated_;
  if (other.allocated_ && other.data_ != nullptr) {
    data_ = new char[size_];
    std::memcpy(data_, other.data_, size_);
  } else {
    data_ = other.data_;
  }

  return *this;
}

// Deserialize a specific value from the byte array
Value Tuple::GetValue(const Schema *schema, uint32_t column_idx) const {
  const Column &col = schema->GetColumn(column_idx);
  uint32_t offset = col.GetOffset();

  // In a real DB we'd check a null bitmap here.
  // For simplicity, we assume values are not null if this is called.

  switch (col.GetType()) {
  case TypeId::INTEGER: {
    int32_t val;
    std::memcpy(&val, data_ + offset, sizeof(int32_t));
    return Value(val);
  }
  case TypeId::BOOLEAN: {
    bool val;
    std::memcpy(&val, data_ + offset, sizeof(bool));
    return Value(val);
  }
  case TypeId::VARCHAR: {
    const char *str_start = data_ + offset;
    size_t len = 0;
    while (len < col.GetLength() && str_start[len] != '\0') {
      len++;
    }
    return Value(std::string(str_start, len));
  }
  default:
    return Value(TypeId::INVALID);
  }
}

const char *Tuple::GetData() const { return data_; }

uint32_t Tuple::GetLength() const { return size_; }
