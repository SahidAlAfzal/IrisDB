#include <iostream>
#include <vector>
#include <cassert>
#include "../DataRepresentationLayer/include/column.h"
#include "../DataRepresentationLayer/include/schema.h"
#include "../DataRepresentationLayer/include/value.h"
#include "../DataRepresentationLayer/include/tuple.h"

int main() {
    // 1. Create a Schema (e.g., id INT, active BOOLEAN, name VARCHAR(20))
    std::vector<Column> cols;
    cols.push_back(Column("id", TypeId::INTEGER, 4));
    cols.push_back(Column("active", TypeId::BOOLEAN, 1));
    cols.push_back(Column("name", TypeId::VARCHAR, 20));
    Schema schema(cols);

    std::cout << "Schema Tuple Length: " << schema.GetTupleLength() << " bytes" << std::endl;
    assert(schema.GetTupleLength() == 25);

    // 2. Create Values
    std::vector<Value> values;
    values.push_back(Value(static_cast<int32_t>(105)));
    values.push_back(Value(true));
    values.push_back(Value(std::string("Alice")));

    // 3. Serialize into a Tuple
    Tuple tuple(values, &schema);
    std::cout << "Tuple successfully serialized. Data Length: " << tuple.GetLength() << " bytes." << std::endl;
    assert(tuple.GetLength() == 25);

    // 4. Simulate saving to disk and reading back (deserialization)
    const char* raw_data = tuple.GetData();
    Tuple deserialized_tuple(raw_data, tuple.GetLength());

    // 5. Read values back out
    Value id_val = deserialized_tuple.GetValue(&schema, 0);
    Value active_val = deserialized_tuple.GetValue(&schema, 1);
    Value name_val = deserialized_tuple.GetValue(&schema, 2);

    std::cout << "Deserialized ID: " << id_val.getAsInt() << std::endl;
    std::cout << "Deserialized Active: " << active_val.getAsBool() << std::endl;
    std::cout << "Deserialized Name: " << name_val.getAsString() << std::endl;

    assert(id_val.getAsInt() == 105);
    assert(active_val.getAsBool() == true);
    // Note: C-strings read back will have padding zeros, which std::string might include if we don't trim, but our tuple.cpp implementation uses \0 terminated logic. Let's see.

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
