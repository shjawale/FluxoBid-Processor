cmake_minimum_required(VERSION 3.28)
project(FluxoBid VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /Ot /arch:AVX2)
else()
    add_compile_options(-ggdb -Wall -Wextra -Wpedantic -O3 -march=native -pthread -fPIC)
endif()


set(SYSTEM_HEADERS iostream vector string_view string stop_token)

function(compile_gxx_header_units HEADERS)
    get_directory_property(CURRENT_DEFS COMPILE_DEFINITIONS)
    foreach(header IN LISTS HEADERS)
        message(STATUS "Pre-compiling C++20 header unit: <${header}>")
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -std=c++20 -fmodules-ts -DASIO_STANDALONE -x c++-system-header ${header}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE res
        )
        if(NOT res EQUAL 0)
            message(FATAL_ERROR "Failed to compile header unit: ${header}")
        endif()
    endforeach()
endfunction()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    compile_gxx_header_units(...)
endif()


include(FetchContent)
find_package(Threads REQUIRED)

# Asio for C++20 Coroutines
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG         asio-1-30-2
)
FetchContent_MakeAvailable(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
    
    add_library(asio_interface INTERFACE)
    
    target_include_directories(asio_interface INTERFACE "${asio_SOURCE_DIR}/asio/include")
    
    target_compile_definitions(asio_interface INTERFACE 
        ASIO_STANDALONE 
        ASIO_NO_TS_EXECUTORS
        ASIO_HAS_STD_COROUTINE=1
    )

    target_compile_definitions(asio_interface INTERFACE ASIO_STANDALONE)
    target_link_libraries(asio_interface INTERFACE Threads::Threads)
endif()

# Catch2 for Unit Testing
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)


# Core Library
add_library(fluxo_core STATIC 
    src/core/bidder.cpp
    src/core/engine.cpp
    src/core/store.cpp
    src/core/server.cpp
    src/core/session.cpp
)

target_include_directories(fluxo_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fluxo_core PUBLIC ASIO_STANDALONE)

add_executable(fluxobid_server src/main.cpp)
target_link_libraries(fluxobid_server PRIVATE fluxo_core)


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(fluxo_core PRIVATE -fmodules-ts -fmodule-mapper=local)
    target_compile_options(fluxo_core PRIVATE "-fmodule-mapper=|@g++-mapper-server -r .")
endif()


set_target_properties(fluxo_core PROPERTIES 
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
)


find_package(yyjson REQUIRED)
target_link_libraries(fluxo_core PRIVATE yyjson)


enable_testing()
add_executable(unit_tests 
    tests/test_main.cpp 
    tests/test_targeting.cpp
    tests/test_ranges.cpp
)
target_link_libraries(unit_tests PRIVATE fluxo_core Catch2::Catch2WithMain)

include(CTest)
add_test(NAME FluxoTests COMMAND unit_tests)


add_executable(bidder_tests 
    tests/test_bidder.cpp
)
target_link_libraries(bidder_tests PRIVATE 
    fluxo_core 
    Catch2::Catch2WithMain
    yyjson
)

# Register with CTest
include(Catch)
catch_discover_tests(bidder_tests)
