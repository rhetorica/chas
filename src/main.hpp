#ifndef MAIN_HPP
    #define MAIN_HPP
    #define VERSION "0.1.0"
    #define NOWHERE (size_t)(-1)
    #define START_ADDRESS 0x0000020000000000

    // do not attempt to change endianness from normal x86 behavior:
    // #define STRAIGHT_ENDIAN
    // #define LITTLE_ENDIAN

    #include <stdio.h>
    #include <string.h>
    #include <malloc.h>

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

    char* get_file_contents(char* filename, fpos_t &size);
    char* get_binary_file_contents(char* filename, fpos_t &size);

    extern size_t base_address;
    extern link<opcode>* opcodes;

#endif // MAIN_HPP