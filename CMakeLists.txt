# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Name your project
project(IrisDB)

# Enforce modern C++17 standards (Crucial for systems programming)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create a variable holding every single .cpp file in your engine
set(SOURCES
    main.cpp
    disk_manager.cpp
    Buffer/BufferPoolManager.cpp
    Buffer/LRUReplacer.cpp
    Buffer/Node.cpp
    Buffer/Page.cpp
    Page/slotted_page.cpp
    indexing/b_plus_tree.cpp
    indexing/b_plus_tree_page.cpp
    indexing/b_plus_internal_page.cpp
    indexing/b_plus_leaf_page.cpp
    DataRepresentationLayer/src/column.cpp
    DataRepresentationLayer/src/schema.cpp
    DataRepresentationLayer/src/value.cpp
    DataRepresentationLayer/src/tuple.cpp
)

# Tell CMake to compile those sources into an executable named 'irisdb'
add_executable(irisdb ${SOURCES})

# Add include directories so files can find headers like "include/column.h"
target_include_directories(irisdb PRIVATE DataRepresentationLayer)