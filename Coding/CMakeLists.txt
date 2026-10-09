cmake_minimum_required(VERSION 4.3)
project(Coding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Coding main.cpp
        handler.cpp
        handler.h)

target_include_directories(Coding PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Тесты GoogleTest: подключаем googletest через FetchContent.
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(CodingTests tests/test_helpers.h
        tests/code_table_test.cpp
        tests/sample_grammar_test.cpp
        tests/data_file_test.cpp
        tests/error_test.cpp
        tests/overflow_test.cpp
        handler.cpp
        handler.h)

target_include_directories(CodingTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_link_libraries(CodingTests PRIVATE GTest::gtest_main GTest::gtest)

include(GoogleTest)
gtest_discover_tests(CodingTests)