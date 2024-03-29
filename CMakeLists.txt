cmake_minimum_required(VERSION 3.10)
project(agi_utils)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_CLANG_TIDY OFF)
if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

add_subdirectory(error_detection)
add_subdirectory(loaders)
add_subdirectory(vserial)
