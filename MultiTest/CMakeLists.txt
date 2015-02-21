cmake_minimum_required(VERSION 2.8.4)
project(MultiTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp base.h base.cpp)
add_executable(MultiTest ${SOURCE_FILES})