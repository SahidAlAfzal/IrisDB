# IrisDB 

> A custom relational database storage engine built from first principles in C++.

## 🎯 Project Objective
IrisDB is an ongoing, educational project designed to deepen my understanding of low-level systems engineering, database internals, and high-performance memory management. Instead of relying on existing frameworks, this project explores how data is physically structured, cached, and retrieved at the hardware level.

*Note: This is an active work-in-progress. Components are being built and tested iteratively.*

## 🏗️ Architecture & Roadmap

The engine is being designed with a modular architecture, focusing on standard RDBMS components.

### 1. Disk Space Manager (Implemented)
- Manages the allocation and deallocation of pages on disk.
- Handles raw File I/O operations to read/write fixed-size pages.

### 2. Buffer Pool Manager (Implemented)
- Manages moving physical pages back and forth from main memory to disk.
- Implements an LRU (Least Recently Used) replacement policy to optimize memory overhead.

### 3. Data Representation & Page Layouts (Implemented)
- Flexible serialization/deserialization of Tuples and Values based on dynamic Schemas.
- Page-level layouts using a slotted page architecture for storing variable-length records.
- Unordered Heap Tables acting as a doubly-linked list of pages for full table scans and storage.

### 4. Access Methods & Indexing (Implemented)
- On-disk B+ Tree indexing supporting highly optimized `O(log n)` data retrieval.
- Internal and Leaf nodes utilizing Buffer Pool Manager for scale-out performance.
- Fully functional API that bridges inserts and selects through the Buffer Pool and B+ Tree index.

### 5. Concurrency Control & Recovery (Planned)
- Implementing latches (Reader-Writer locks) to ensure thread safety and prevent race conditions during simultaneous page access.
- Write-Ahead Logging (WAL) and recovery mechanisms for data durability (ACID compliance).

## 🛠️ Tech Stack
* **Language:** C++ (Modern C++17/20)
* **Build System:** CMake
* **Testing:** Google Test (GTest)

## 🧠 Why Build This?
Modern backend applications (like payment gateways and high-concurrency booking engines) demand rigorous data integrity and sub-millisecond latencies. By building the underlying storage layer from scratch, I am strengthening my ability to debug complex race conditions, optimize I/O operations, and write performance-critical C++ code.