cmake_minimum_required(VERSION 3.22)
project(VQ)

set(CMAKE_CXX_STANDARD 14)

add_executable(VQ main.cpp VQ.cpp ImageReader.cpp)
