cmake_minimum_required(VERSION 3.3)
project(grandeur)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp mechanics.cpp mechanics.h gems.cpp gems.h)
add_executable(grandeur ${SOURCE_FILES})