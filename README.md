# Apex-Async-Bidder

A high-performance, asynchronous Real-Time Bidding (RTB) engine built with **C++20**. Designed for ultra-low latency (<1ms internal) and high-concurrency environments, this project implements a production-grade bidding core capable of processing thousands of [OpenRTB 2.5](https://www.iab.com) requests per second.

##  Key Architectural Features
*   **Zero-Copy Design**: Leverages `std::span` and `std::string_view` throughout the hot path to eliminate heap allocations and redundant data copies during bid processing.
*   **Coroutine-Based I/O**: Utilizes **C++20 Coroutines** to handle massive request concurrency with minimal thread-context-switching overhead.
*   **Lock-Free Budget Pacing**: Implements a thread-safe, lock-free campaign budget tracker using `std::atomic` to avoid mutex contention in multi-threaded bidding cycles.
*   **SIMD Accelerated Parsing**: Integrates [simdjson](https://github.com) to parse multi-kilobyte JSON payloads at gigabyte-per-second speeds using AVX2/AVX-512 instructions.
*   **Memory Pooling**: Uses a custom **Pool Allocator** (or `std::pmr`) for campaign matching objects to ensure deterministic performance and cache friendliness.

##  Tech Stack & Requirements
*   **Language**: C++20 (utilizing Modules, Concepts, and Coroutines)
*   **Build System**: [CMake 3.28+](https://cmake.org) & [Ninja](https://ninja-build.org)
*   **JSON Engine**: `simdjson`
*   **Testing**: [Catch2 v3](https://github.com)
*   **Benchmarking**: [Google Benchmark](https://github.com)

##  Project Structure
```text
Apex-Async-Bidder/
├── benchmarks/    # Performance testing suite (Google Benchmark)
├── include/       # Public API & C++20 Module interfaces
├── src/           # Core logic (Networking, Matching Engine, Allocators)
├── tests/         # Unit tests (Catch2)
├── third_party/   # External libraries (simdjson, etc.)
└── main.cpp       # Application entry point
