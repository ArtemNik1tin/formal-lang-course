cmake_minimum_required(VERSION 4.3)
project(Coding)

set(CMAKE_CXX_STANDARD 20)

add_executable(Coding main.cpp
        handler.cpp
        handler.h)

target_include_directories(Coding PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
