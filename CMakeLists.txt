cmake_minimum_required(VERSION 3.2.2)
project(grandeur)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp move.cpp move.h gems.cpp gems.h card.cpp card.h board.cpp board.h constants.h noble.h noble.cpp)
add_executable(grandeur ${SOURCE_FILES})

add_subdirectory(tests)