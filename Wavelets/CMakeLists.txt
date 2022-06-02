cmake_minimum_required(VERSION 3.22)
project(Wavelets)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-O5")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )


add_executable(Encode encode.cpp sub.c fqmf16AA.c fqmf16AS.c ImageReader.cpp WaveletHelper.cpp VQ.cpp)
add_executable(Decode decode.cpp sub.c fqmf16AA.c fqmf16AS.c ImageReader.cpp WaveletHelper.cpp VQ.cpp)
add_executable(Treino treino.cpp sub.c fqmf16AA.c fqmf16AS.c ImageReader.cpp WaveletHelper.cpp VQ.cpp)
add_executable(Teste teste.cpp sub.c fqmf16AA.c fqmf16AS.c ImageReader.cpp WaveletHelper.cpp VQ.cpp)
