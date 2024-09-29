cmake_minimum_required(VERSION 3.13)
project(chasm-assembler)
set(APP_NAME "chas")
set(CMAX_CXX_STANDARD 14)

set(SOURCES src/main.cpp src/type/opcode.cpp src/type/token.cpp src/type/symbol.cpp src/type/register.cpp)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-char-subscripts)

add_executable(${APP_NAME} ${SOURCES})
