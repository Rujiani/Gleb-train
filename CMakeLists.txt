cmake_minimum_required(VERSION 3.31.2)

project(my_great_train_library)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)

add_subdirectory(train)
add_subdirectory(van)
add_subdirectory(test)

add_executable(main main.cpp)

target_link_libraries(main van train)
