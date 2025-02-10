cmake_minimum_required(VERSION 3.31.2)

project(tests VERSION 1.0.0 DESCRIPTION "Test for my library" LANGUAGES CXX)

add_executable(tests test.cpp)

target_compile_options(tests PRIVATE -g)

find_package(Catch2 3 REQUIRED)

target_link_libraries(tests train van Catch2::Catch2WithMain)
