#ifndef VALUE_H
#define VALUE_H

#include <cstdint>
#include <string>
#include <variant>

enum class TypeId {
    INVALID = 0,
    BOOLEAN,
    INTEGER,
    VARCHAR
};

class Value {
private:
    TypeId type_id_;
    std::variant<std::monostate, bool, int32_t, std::string> value_;
    bool is_null_;

public:
    // Constructors
    explicit Value(int32_t val);
    explicit Value(bool val);
    explicit Value(const std::string& val);

    // Constructor for NULL values
    explicit Value(TypeId type);

    // Getters
    TypeId getType() const;
    bool isNull() const;

    // Value accessors
    int32_t getAsInt() const;
    bool getAsBool() const;
    std::string getAsString() const;
};

#endif // VALUE_H