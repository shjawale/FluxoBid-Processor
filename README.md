# FluxoBid: High-Performance OpenRTB 2.5 Core

A high-performance, asynchronous Real-Time Bidding (RTB) engine built with C++20. Designed for ultra-low latency (<1ms internal) and high-concurrency environments, this project implements a production-grade bidding core capable of processing thousands of [OpenRTB 2.5](https://www.iab.com) requests per second.

## Key Architectural Features (Phase 1: Ingestion & Validation)
*    **Zero-Copy JSON Ingestion**: Leverages ```std::string_view``` and the high-performance ```yyjson``` library to map incoming OpenRTB requests directly to C++ structs without heap allocations or redundant string copying.
*    **Hybrid C++20 Module Strategy**: Implements a robust architectural split using C++20 Modules for core business logic while utilizing a Global Module Fragment to shield against compiler-specific limitations in third-party networking headers.
*    **Multi-Impression Support**: Fully compliant with the OpenRTB 2.5 spec, allowing the engine to parse, validate, and store multiple imp objects (Banner/Video slots) per individual request.
*    **Early-Exit Validation**: Includes a "Pre-Flight" validation layer that identifies "deal-breakers" (invalid IDs, low timeouts, or privacy restrictions) to trigger instant HTTP 204 No Content responses, preserving CPU for high-value auctions.
*    **Type-Safe Modeling**: Uses C++20 Concepts and ```std::optional``` to enforce strict schema adherence for complex nested objects like Device, Site, and Banner configurations.

## Tech Stack & Requirements
*    **Language**: C++20 (utilizing Modules, Concepts, and ```std::optional```)
*    **Build System**: CMake 3.28+
*    **JSON Engine**: yyjson (C-based for maximum performance)
*    **Networking**: [Asio](https://think-async.com/Asio/) (Standalone version)
*    **Testing**: [Catch2 v3](https://github.com/catchorg/Catch2)

## Project Structure
```
FluxoBid-Processor/
├── include/
│   └── bidder.hpp         # Header for bidder
├── src/
│   └── core/
│       └── bidder.cpp     # Logic implementation & JSON Parsing
├── tests/
│   └── test_bidder.cpp    # Unit Test suite (Catch2) verifying OpenRTB compliance
└── CMakeLists.txt         # Modern CMake build with C++20 Module support
```

## Current Progress: Verified Parsing & Unit Testing
The project has successfully reached the Ingestion Milestone. The test_bidder suite validates:
*    Correct mapping of raw OpenRTB 2.5 JSON to internal C++ structs.
*    Proper handling of std::string_view lifetimes relative to the yyjson document.
*    Graceful failure and rejection of malformed or logically invalid bid requests.
