cmake_minimum_required(VERSION 3.21)
project(PPM C)

set(CMAKE_C_STANDARD 99)

add_executable(PPM main.c)
add_executable(encode ppm.c encode.c bit_output.c arithmetic_encode.c adaptive_model.c)
add_executable(decode ppm.c decode.c bit_input.c arithmetic_decode.c adaptive_model.c)
