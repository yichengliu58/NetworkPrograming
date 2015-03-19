cmake_minimum_required(VERSION 3.1)
project(Sort)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(Sort ${SOURCE_FILES})