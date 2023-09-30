cmake_minimum_required(VERSION 3.10)
project(assembler)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(assembler
    SHARED
    assembler.cpp 
    lib/json/jsoncpp.cpp
) # 生成动态库文件

SET_TARGET_PROPERTIES(assembler PROPERTIES OUTPUT_NAME assembler)
