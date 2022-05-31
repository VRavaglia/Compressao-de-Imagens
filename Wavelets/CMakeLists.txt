cmake_minimum_required(VERSION 3.22)
project(Wavelets)

set(CMAKE_CXX_STANDARD 17)

add_executable(Encode encode.cpp sub.c fqmf16AA.c fqmf16AS.c)
add_executable(Decode decode.cpp sub.c fqmf16AA.c fqmf16AS.c)
