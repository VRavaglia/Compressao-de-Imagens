cmake_minimum_required(VERSION 3.22)
project(VQ)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-O5")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(Train train.cpp VQ.cpp ImageReader.cpp)
add_executable(Test test.cpp VQ.cpp ImageReader.cpp)
