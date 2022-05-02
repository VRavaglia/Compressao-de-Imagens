cmake_minimum_required(VERSION 3.21)
project(PPM C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=undefined -fsanitize-memory-track-origins -g -O3")

add_executable(PPM main.c)
add_executable(encode encode.c bit_output.c arithmetic_encode.c ppm.c)
add_executable(decode decode.c bit_input.c arithmetic_decode.c ppm.c)
