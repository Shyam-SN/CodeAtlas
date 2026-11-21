cmake_minimum_required(VERSION 3.16)
project(CodeAtlas VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warning and optimization flags
include(CheckCXXCompilerFlag)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -O3)
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)")
        check_cxx_compiler_flag("-mcpu=native" COMPILER_SUPPORTS_MCPU_NATIVE)
        if(COMPILER_SUPPORTS_MCPU_NATIVE)
            add_compile_options(-mcpu=native)
        endif()
    else()
        check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
        if(COMPILER_SUPPORTS_MARCH_NATIVE)
            add_compile_options(-march=native)
        endif()
    endif()
endif()

# Include directories
include_directories(include)

# Find LLVM and Clang
if(NOT DEFINED LLVM_DIR AND EXISTS "/opt/homebrew/opt/llvm/lib/cmake/llvm")
    set(LLVM_DIR "/opt/homebrew/opt/llvm/lib/cmake/llvm" CACHE PATH "Path to LLVM CMake config")
endif()
if(NOT DEFINED Clang_DIR AND EXISTS "/opt/homebrew/opt/llvm/lib/cmake/clang")
    set(Clang_DIR "/opt/homebrew/opt/llvm/lib/cmake/clang" CACHE PATH "Path to Clang CMake config")
endif()
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Create library
add_library(codeatlas STATIC
    src/model/repository_model.cpp
    src/parser/parser_engine.cpp
    src/parser/cpp_parser.cpp
    src/parser/python_parser.cpp
    src/parser/cmake_parser.cpp
    src/parser/javascript_parser.cpp
    src/graph/knowledge_graph.cpp
    src/graph/graph_builder.cpp
    src/layout/force_directed.cpp
    src/layout/hierarchical_layout.cpp
    src/ai/ai_assistant.cpp
    src/ai/repository_indexer.cpp
    src/ai/vector_database.cpp
    src/ai/security_scanner.cpp
    src/utils/file_utils.cpp
)

target_link_libraries(codeatlas PUBLIC libclang)

target_include_directories(codeatlas PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable Testing
enable_testing()
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Fetch Google Benchmark
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.tar.gz
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

# Add Subdirectories
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(examples)
add_subdirectory(qt_app)
