# IrisDB API Reference

This document provides a high-level map of the core components of IrisDB, detailing their classes, internal attributes, and public APIs.

## 1. Disk Management Layer
Handles direct reading and writing of fixed-size chunks (pages) from the OS file system.

| Class | Attributes | Methods | Description |
|---|---|---|---|
| `DiskManager` | `db_file`, `filename`, `num_pages` | `WritePage(page_id, page_data)`<br>`ReadPage(page_id, page_data)`<br>`GetFileSize()`<br>`AllocatePage()` | Manages raw I/O for fixed-size 4KB pages on the filesystem. |

## 2. Buffer Pool Management Layer
Acts as a bridge between main memory (RAM) and disk. Keeps hot pages in memory to reduce I/O latency.

| Class | Attributes | Methods | Description |
|---|---|---|---|
| `BufferPoolManager` | `disk_manager_`, `replacer_`, `pages_`, `page_table_`, etc. | `FetchPage(page_id)`<br>`NewPage(&page_id)`<br>`UnpinPage(page_id, is_dirty)`<br>`FlushPage(page_id)`<br>`DeletePage(page_id)` | Fetches pages, delegates caching via LRU, and handles write-back. |
| `LRUReplacer` | LRU linked list / hash map | `Victim(&frame_id)`<br>`Pin(frame_id)`<br>`Unpin(frame_id)`<br>`Size()` | Tracks page usage to determine which page to evict when pool is full. |
| `Page` | `data_[4096]`, `page_id_`, `pin_count_`, `is_dirty_`, `rwlatch_` | `GetPageId()`<br>`GetData()`<br>`GetPinCount()`<br>`IsDirty()` | In-memory wrapper for a 4KB chunk of raw data. |

## 3. Data Representation Layer
Handles how high-level SQL data types (Integers, Strings) are mapped to schemas and serialized into raw bytes.

| Class | Attributes | Methods | Description |
|---|---|---|---|
| `Value` | `type_id_`, `value_` (std::variant), `is_null_` | `getType()`, `isNull()`, `getAsInt()`, `getAsBool()`, `getAsString()` | High-level representation of a single typed value. |
| `Tuple` | `size_`, `data_`, `allocated_` | `GetValue(schema, col_idx)`<br>`GetData()`, `GetLength()` | Represents a single row of data serialized into a byte array. |
| `Column` | `column_name_`, `column_type_`, `fixed_length_`, `variable_length_`, `column_offset_` | Getters for attributes. | Metadata for a single column in a table. |
| `Schema` | `columns_`, `tuple_length_` | `GetColumns()`, `GetColumn(idx)`, `GetTupleLength()` | Metadata describing the exact layout of a Tuple. |

## 4. Page Layout & Heap Table Layer
Organizes serialized Tuples within physical Pages (Slotted Page) and groups Pages into an unordered table (Heap Table).

| Class | Attributes | Methods | Description |
|---|---|---|---|
| `SlottedPage` | `page_data` | `Init(page_id)`<br>`InsertTuple(data, size)`<br>`deleteTuple(slot_id)`<br>`getTuple(slot_id)` | Structures a 4KB page into headers, slots, and dynamically sized tuple payloads. |
| `HeapTable` | `bpm_`, `first_page_id_`, `last_page_id_` | `InsertTuple(tuple, &rid)`<br>`GetTuple(rid, &tuple)` | A doubly-linked list of pages managing a complete table of raw tuples. |
| `RecordID` | `page_id_`, `slot_id_` | `operator==` | Uniquely identifies the physical location of a Tuple on disk. |

## 5. Indexing Layer
Provides `O(log n)` access methods via an On-Disk B+ Tree.

| Class | Attributes | Methods | Description |
|---|---|---|---|
| `BPlusTreePage` | `page_type_`, `size_`, `max_size_`, `parent_page_id_`, `page_id_` | Setters & Getters for attributes. | Base class for B+ Tree nodes (headers). |
| `BPlusTreeLeafPage` | `next_page_id_`, `array_` (Key->RID map) | `Insert(key, rid)`<br>`Lookup(key, &rid)`<br>`MoveHalfTo()` | Leaf node holding actual Key-to-RecordID mappings. |
| `BPlusTreeInternalPage`| `array_` (Key->PageID map) | `Lookup(key)`<br>`Insert(key, page_id)`<br>`MoveHalfTo()` | Internal node used for routing searches to leaf nodes. |
| `BPlusTree` | `root_page_id_`, `bpm_` | `Insert(key, rid)`<br>`GetValue(key, &rid)` | Top-level API managing splits, merges, and root node updates. |
