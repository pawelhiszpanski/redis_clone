# C++ Redis Clone (In-Memory Database)

A fast, lightweight, and crash-resilient in-memory Key-Value database written in modern C++. This project successfully replicates core architectural patterns found in production Redis environments, focusing on memory safety, constant-time complexity, and data persistence.

## 🚀 Key Features

* **In-Memory Hash Storage:** Blazing fast `O(1)` read/write operations using `std::unordered_map`.
* **Constant-Time O(1) LRU Cache:** Implements a highly optimized Least Recently Used eviction policy. It avoids slow `O(N)` list traversals by storing `std::list` iterators directly inside the value structure, enabling instantaneous cache updates and evictions.
* **AOF (Append-Only File) Persistence:** Fully crash-resilient. Every mutating command (`SET`, `DEL`, `RENAME`) is logged sequentially to a disk file. Deletions are handled using **Tombstone** records (`DELETE key`).
* **AOF Rewrite (Log Compression):** Users can trigger the `REWRITE` command to squash massive AOF history logs into a single, clean snapshot of the current RAM state, saving disk space and reducing boot time.
* **Lazy Expiration (TTL):** Keys can be assigned an expiration time (`EXPIRE`). The system uses a lazy-evaluation approach, checking and securely garbage-collecting expired keys only when they are accessed or during routine background checks.

## 🛠️ Architecture Highlight: O(1) LRU

The database completely avoids linear memory scanning. The core data structure is defined as:

```cpp
class RedisValue {
public:
    std::string value;
    long long expireAt;
    std::list<std::string>::iterator lru_ptr; // Direct pointer to the LRU node
};
```

By binding the Hash Map and the Doubly-Linked List together, any `GET` or `SET` operation can "teleport" directly to the correct memory address in the LRU queue and update its position in true O(1) time.

## 💻 Supported Commands

| Command | Syntax | Description |
| :--- | :--- | :--- |
| **SET** | `SET <key> <value>` | Inserts or updates a key-value pair. Resets TTL. |
| **GET** | `GET <key>` | Retrieves the value of a key and updates its LRU status. |
| **DEL** | `DEL <key>` | Safely removes a key from RAM and writes a tombstone to AOF. |
| **RENAME**| `RENAME <old> <new>` | Renames an existing key while preserving its value and TTL. |
| **EXISTS**| `EXISTS <key>` | Checks if a key exists and hasn't expired. |
| **EXPIRE**| `EXPIRE <key> <sec>` | Sets a Time-To-Live for a specific key in seconds. |
| **REWRITE**| `REWRITE` | Compresses the AOF log file to free up disk space. |
| **KEYS** | `KEYS` | Lists all currently valid keys in the database. |
| **DBSIZE**| `DBSIZE` | Returns the total number of valid keys. |
| **NEW** | `NEW` | Performs a hard flush, wiping all RAM data and the AOF file. |

## ⚙️ Build and Run

This project uses standard C++ libraries and requires a compiler supporting C++17 or higher.

```bash
# Compile the source code
g++ -std=c++17 main.cpp -o redis_clone

# Run the database
./redis_clone
```
