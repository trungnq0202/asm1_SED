cmake_minimum_required(VERSION 3.15)
project(asm1_SED)

set(CMAKE_CXX_STANDARD 14)

add_executable(asm1_SED main.cpp Stack.cpp Stack.h Queue.cpp Queue.h Node.cpp Node.h)