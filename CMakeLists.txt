cmake_minimum_required(VERSION 3.6)
project(Project4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES pgir.cpp parser.cc)
add_executable(Project4 ${SOURCE_FILES})