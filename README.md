# FluxoBid: High-Performance OpenRTB 2.5 Core

A high-performance, asynchronous Real-Time Bidding (RTB) engine built with C++20. Designed for ultra-low latency (<1ms internal) and high-concurrency environments, this project implements a bidding core capable of processing hundreds of [OpenRTB 2.5](https://www.iab.com) requests per second.

## Key Architectural Features
*   **Zero-Copy JSON Ingestion**: Leverages `std::string_view` and **yyjson** to map OpenRTB requests directly to C++ structs without heap allocations or redundant data copies.
*   **Hybrid C++20 Module Strategy**: Utilizes a robust architectural split with **Named Modules** for business logic and **Global Module Fragments** to maintain stability with third-party networking headers (Asio).
*   **Early-Exit Validation**: A "Pre-Flight" layer identifies "deal-breakers" (missing IDs, low timeouts) to trigger instant **HTTP 204 No Content** responses, preserving CPU for high-value auctions.
*   **Type-Safe Modeling**: Employs **C++20 Concepts** and `std::optional` to enforce strict schema adherence for complex nested objects like `Device`, `Site`, and `Banner` configurations.
*   **Asynchronous I/O Stack**: Implements a high-concurrency Server/Session pattern using **Asio Coroutines** to manage thousands of simultaneous TCP connections.


## Tech Stack & Requirements
*    **Language**: C++20 (utilizing Concepts, and Coroutines)
*    **Compiler**: Clang 16+ (optimized for diagnostics and C++20 standard compliance)
*    **Build System**: CMake 3.28+
*    **JSON Engine**: yyjson (C-based for maximum performance)
*    **Networking**: [Asio](https://think-async.com/Asio/) (Standalone version)
*    **Testing**: [Catch2 v3](https://github.com/catchorg/Catch2)

## Project Structure
```
FluxoBid-Processor/
├── include/                # Public API, Models, and Concepts
├── src/
│   └── core/               # Bidder, Engine, and CampaignStore logic
│   └── network/            # Server and Session connection management
├── tests/                  # Unit Test suite (Catch2) verifying OpenRTB compliance
│   ├── test_bidder.cpp     
│   └── test_engine.cpp     
├── CMakeLists.txt          # Modern CMake build with C++20 Module support
└── main.cpp                # Application entry point & Asio event loop
```

## Current Progress: Verified Parsing & Unit Testing
The project has successfully reached the Ingestion & Decisioning Milestone, moving from raw network bytes to a validated bidding decision.

*    OpenRTB Ingestion & Auction Logic: Successfully implemented a "Sieve" matcher that parses multi-impression OpenRTB 2.5 payloads and filters campaigns based on Geography, Size, and Price Floor.
*    Asynchronous Network Core: Developed a custom Server/Session architecture using Clang 16+ and Asio to handle concurrent TCP connections with minimal overhead.
*    Protocol Handling: Supports the standard OpenRTB 2.5 response lifecycle, including automated HTTP 204 "No-Bid" signals and dynamic JSON BidResponse generation via yyjson.
*    Automated Verification: Core logic is verified via a Catch2 v3 test suite (test_bidder) that validates stable field mapping, campaign matchmaking, and memory safety across the pipeline.
