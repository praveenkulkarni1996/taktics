cmake_minimum_required(VERSION 3.4)
project(taktics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Ofast -march=native -std=c++11")

set(SOURCE_FILES utility.cpp board.cpp player.cpp)
add_executable(taktics ${SOURCE_FILES})