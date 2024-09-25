#ifndef MAIN_HPP
    #define MAIN_HPP
    #define VERSION "0.0.2"
    #define NOWHERE (size_t)(-1)
    #define START_ADDRESS 0x0000020000000000

    #include <stdio.h>
    #include <string.h>
    #include <malloc.h>
    #include "type/string.hpp"

    template <typename T>
    class link {
    public:
        T* child = NULL;
        link* next = NULL;
    };

    struct token;
    struct label;

    #include "type/register.h"
    #include "type/opcode.h"
    #include "type/symbol.h"
    #include "type/token.h"

    extern size_t base_address;
    extern link<opcode>* opcodes;

#endif // MAIN_HPP