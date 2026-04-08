# IrisDB 

> A custom relational database storage engine built from first principles in C++.

## 🎯 Project Objective
IrisDB is an ongoing, educational project designed to deepen my understanding of low-level systems engineering, database internals, and high-performance memory management. Instead of relying on existing frameworks, this project explores how data is physically structured, cached, and retrieved at the hardware level.

*Note: This is an active work-in-progress. Components are being built and tested iteratively.*

## 🏗️ Architecture & Roadmap

The engine is being designed with a modular architecture, focusing on standard RDBMS components.

### 1. Disk Space Manager (Current Focus)
- Manages the allocation and deallocation of pages on disk.
- Handles raw File I/O operations to read/write fixed-size pages.

### 2. Buffer Pool Manager (In Progress)
- Manages moving physical pages back and forth from main memory to disk.
- Implementing an LRU (Least Recently Used) replacement policy to optimize memory overhead.
- Ensuring thread-safe access to cached pages.

### 3. Access Methods (Planned)
- Designing B+ Tree indexing for highly optimized `O(log n)` data retrieval and range queries.
- Implementing page-level layouts (slotted page architecture) for storing variable-length records.

### 4. Concurrency Control (Planned)
- Implementing latches (Reader-Writer locks) to ensure thread safety and prevent race conditions during simultaneous page access.

## 🛠️ Tech Stack
* **Language:** C++ (Modern C++17/20)
* **Build System:** CMake
* **Testing:** Google Test (GTest)

## 🧠 Why Build This?
Modern backend applications (like payment gateways and high-concurrency booking engines) demand rigorous data integrity and sub-millisecond latencies. By building the underlying storage layer from scratch, I am strengthening my ability to debug complex race conditions, optimize I/O operations, and write performance-critical C++ code.