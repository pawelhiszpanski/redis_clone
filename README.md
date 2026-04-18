# Redis Clone: Lightweight Key-Value Store

A lightweight, persistent key-value database built from scratch in modern C++. This project mimics the core mechanics of Redis, operating via a Command Line Interface (CLI) and ensuring data persistence across sessions using file streams.

## Tech Stack
* **Language:** C++17
* **Core Libraries:** `<unordered_map>`, `<fstream>`, `<sstream>`
* **Architecture:** Procedural with separated command handler functions for maintainability.

## Key Features
* **O(1) Time Complexity:** Utilizes `std::unordered_map` for fast data retrieval and insertion.
* **Data Persistence:** Automatically loads existing databases on startup and safely truncates/saves data on exit using `std::ifstream` and `std::ofstream`.
* **Robust Input Parsing:** Uses `std::stringstream` and whitespace manipulation (`std::ws`) to safely handle multi-word values (e.g., storing full sentences under a single key).
* **Safe Operations:** Includes user confirmations for destructive commands (like flushing the entire database).

## Available Commands
* `SET <key> <value>` - Inserts or updates a key-value pair.
* `GET <key>` - Retrieves the value associated with the key.
* `DEL <key>` - Safely removes a key from the database.
* `NEW` - Flushes the entire database (RAM and Disk) with a safety prompt.
* `CLEAR` - Clears the terminal view.
* `EXIT` / `QUIT` - Safely saves the current state to disk and terminates the engine.
