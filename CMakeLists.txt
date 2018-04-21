cmake_minimum_required(VERSION 3.1)
project(assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(assembler parser-main.cpp lexer.cpp parser.cpp)
add_executable(Assembler ${assembler})