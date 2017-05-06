cmake_minimum_required(VERSION 3.1)

set(LLVM_PATH /usr/lib/llvm-3.9)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(src)  # Use your pass name here.
