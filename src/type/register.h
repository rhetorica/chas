
#ifndef REGISTER_H
#define REGISTER_H
    #define REGISTER_COUNT 16
    struct reg {
        const char* name;
        char number;
    };

    void init_registers();
    extern link<reg>* regs;
    reg* find_register(char* text);
#endif // REGISTER_H
