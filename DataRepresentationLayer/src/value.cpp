#include "../include/value.h"

#include <stdexcept>

Value::Value(int32_t val) {
    type_id_ = TypeId::INTEGER;
    value_ = val;
    is_null_ = false;
}

Value::Value(bool val) {
    type_id_ = TypeId::BOOLEAN;
    value_ = val;
    is_null_ = false;
}

Value::Value(const std::string& val) {
    type_id_ = TypeId::VARCHAR;
    value_ = val;
    is_null_ = false;
}

// Constructor for NULL
Value::Value(TypeId type) {
    type_id_ = type;
    value_ = std::monostate{};
    is_null_ = true;
}

TypeId Value::getType() const {
    return type_id_;
}

bool Value::isNull() const {
    return is_null_;
}

int32_t Value::getAsInt() const {
    if (is_null_ || type_id_ != TypeId::INTEGER)
        throw std::logic_error("Not an INTEGER");

    return std::get<int32_t>(value_);
}

bool Value::getAsBool() const {
    if (is_null_ || type_id_ != TypeId::BOOLEAN)
        throw std::logic_error("Not a BOOLEAN");

    return std::get<bool>(value_);
}

std::string Value::getAsString() const {
    if (is_null_ || type_id_ != TypeId::VARCHAR)
        throw std::logic_error("Not a VARCHAR");

    return std::get<std::string>(value_);
}