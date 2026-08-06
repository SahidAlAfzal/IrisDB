### The Architecture in Action  
If you execute `CREATE TABLE users (id INTEGER, active BOOLEAN);`, the engine does this:

1. Creates `Column("id", TypeId::INTEGER, 4)`

2. Creates `Column("active", TypeId::BOOLEAN, 1)`

3. Passes them to Schema.

The Schema calculates that id starts at byte offset 0, and active starts at byte offset 4. The total tuple_length_ is 5 bytes.