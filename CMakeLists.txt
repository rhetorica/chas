cmake_minimum_required(VERSION 3.13)
project(chasm-assembler)
set(CMAX_CXX_STANDARD 14)

set(SOURCE_FILES src/main.cpp src/type/opcode.cpp src/type/token.cpp src/type/symbol.cpp src/type/register.cpp)

add_executable(chas ${SOURCE_FILES})
