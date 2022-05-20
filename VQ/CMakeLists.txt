cmake_minimum_required(VERSION 3.22)
project(VQ)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(VQ main.cpp VQ.cpp ImageReader.cpp)
