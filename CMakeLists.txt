cmake_minimum_required(VERSION 3.20)
project(FluxoBid VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Use strict C++20 for portability

# Low-latency optimization flags
if(MSVC)
    add_compile_options(/W4 /Ot /arch:AVX2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native)
endif()


include(FetchContent)
find_package(Threads REQUIRED)

# Asio for C++20 Coroutines
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
    
    # Manually define the INTERFACE target
    add_library(asio_interface INTERFACE)
    
    # Point to the actual header directory within the fetched content
    target_include_directories(asio_interface INTERFACE "${asio_SOURCE_DIR}/asio/include")
    
    # Set required definitions and link Threads
    target_compile_definitions(asio_interface INTERFACE ASIO_STANDALONE)
    target_link_libraries(asio_interface INTERFACE Threads::Threads)
endif()

# Catch2 for Professional Unit Testing
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)


# Core Library (FluxoBid Engine Logic)
add_library(fluxo_core STATIC 
    src/core/bidder.cpp
    src/core/engine.cpp
)

target_include_directories(fluxo_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include>
    $<INSTALL_INTERFACE:include>
)

# Link Asio (header-only) to the core
#target_link_libraries(fluxo_core PRIVATE asio_interface)
target_compile_definitions(fluxo_core PUBLIC ASIO_STANDALONE)

# Main Executable (The FluxoBid Server)
add_executable(fluxobid_server src/main.cpp)
target_link_libraries(fluxobid_server PRIVATE fluxo_core)


enable_testing()
add_executable(unit_tests 
    tests/test_main.cpp 
    tests/test_targeting.cpp
    tests/test_ranges.cpp
)
target_link_libraries(unit_tests PRIVATE fluxo_core Catch2::Catch2WithMain)

include(CTest)
add_test(NAME FluxoTests COMMAND unit_tests)
