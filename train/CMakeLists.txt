cmake_minimum_required(VERSION 3.31.2)

add_library(train train.hpp train.cpp)

target_link_libraries(train van)