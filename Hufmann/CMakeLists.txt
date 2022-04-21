cmake_minimum_required(VERSION 3.21)
project(Huffman)

set(CMAKE_CXX_STANDARD 14)

add_executable(Huffman main.cpp)
add_executable(Encoder encoder.cpp)
add_executable(Decoder decoder.cpp)
