#include "../main.hpp"
#include "opcode.h"

link<opcode>* opcodes = NULL;

link<opcode>* add_opcode(link<opcode>* last, unsigned short number, const char* text, unsigned short emission, int format) {
    link<opcode>* new_opcode = new link<opcode>;
    last->next = new_opcode;
    new_opcode->child = new opcode;
    new_opcode->child->number = number;
    new_opcode->child->text = text;
    new_opcode->child->emission = emission;
    new_opcode->child->format = format;
    new_opcode->next = NULL;
    return new_opcode;
}

opcode* find_opcode(char* text) {
    link<opcode>* o = opcodes;
    while(o != NULL) {
        if(strcmp(text, o->child->text) == 0) {
            return o->child;
        }
        // printf(" -- checking %s vs %s\n", text, o->child->text);
        o = o->next;
    }
    return NULL;
}

void init_opcodes() {
    unsigned short n = 0;

    opcodes = new link<opcode>();
    opcodes->child = new opcode();
    opcodes->child->number = 0;
    opcodes->child->text = "null";
    opcodes->child->emission = 0x0000;
    opcodes->child->format = O_ZERO;
    opcodes->next = NULL;
    link<opcode>* last_opcode = opcodes;

    last_opcode = add_opcode(last_opcode, n++, "push",    0x0180, O_MONO_OR_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "push.q",  0x0180, O_MONO_OR_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "push.d",  0x0140, O_MONO_OR_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "push.w",  0x0120, O_MONO_OR_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "pop",     0x0280, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "pop.q",   0x0280, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "pop.d",   0x0240, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "pop.w",   0x0220, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "push.a",  0x013F, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "pop.a",   0x023F, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "push.h",  0x011F, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "pop.h",   0x021F, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "load.a",  0x0430, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "load.h",  0x0410, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "store.a", 0x0830, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "store.h", 0x0810, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "cdr",     0x0B00, O_MONO | O_DUO | O_STATUS_FORBIDDEN);
    last_opcode = add_opcode(last_opcode, n++, "flip",    0x0B0F, O_DUO_ONLY);
    last_opcode = add_opcode(last_opcode, n++, "copy",    0x0C00, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "unpack",  0x0D00, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "pack",    0x0E00, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "swap",    0x0F00, O_MONO | O_DUO);

    last_opcode = add_opcode(last_opcode, n++, "load.w",  0x1000, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "load.d",  0x1100, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "load",    0x1200, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "load.wi", 0x1310, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "load.di", 0x1320, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "load.i",  0x1340, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);

    last_opcode = add_opcode(last_opcode, n++, "set.w", 0x1390, O_MONO | O_LIT_16 | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "set.d", 0x13A0, O_MONO | O_LIT_32 | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "set",  0x13C0, O_MONO | O_LIT_64 | O_MONO_AND_LIT);

    last_opcode = add_opcode(last_opcode, n++, "store.w", 0x1400, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "store.d", 0x1500, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "store",   0x1600, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "store.wi",0x1710, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "store.di",0x1720, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);
    last_opcode = add_opcode(last_opcode, n++, "store.i", 0x1740, O_MONO | O_LIT_64 | O_LIT_REF | O_MONO_AND_LIT);

    // badly-spaced:
    /*last_opcode = add_opcode(last_opcode, n++, "get.w",   0x1100, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "get.d",   0x1200, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "get",     0x1400, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "get.q",   0x1400, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "put.w",   0x1900, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "put.d",   0x1A00, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "put",     0x1C00, O_MONO | O_DUO);
    last_opcode = add_opcode(last_opcode, n++, "put.q",   0x1C00, O_MONO | O_DUO);*/
    last_opcode = add_opcode(last_opcode, n++, "disable", 0x2000, O_FLAG | O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "enable",  0x2400, O_FLAG | O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "bcopy",   0x5000, O_MONO | O_DUO | O_TRIO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bfill",   0x7000, O_MONO | O_DUO | O_TRIO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "update",  0x3000, O_MONO | O_DUO | O_TRIO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "jmp",     0x6000, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jne",     0x6040, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jc",      0x4010, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jnc",     0x6010, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jge",     0x40C0, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "je",      0x4040, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jle",     0x4060, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jg",      0x4080, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jl",      0x4020, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jz",      0x4100, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jnz",     0x6100, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "js",      0x4200, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "jns",     0x6200, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.n",   0x1800, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.wr",  0x1810, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.ws",  0x1820, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.wrs", 0x1830, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.dr",  0x1850, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.ds",  0x1860, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.drs", 0x1870, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.qr",  0x1890, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.qs",  0x18A0, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "rep.qrs", 0x18B0, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "int",     0xF000, O_OPT_IMM_8);
    last_opcode = add_opcode(last_opcode, n++, "iret",    0xF401, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "lmmt",    0xF410, O_MONO_OR_LIT | O_LIT_64 | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "ret",     0xF400, O_ZERO);
    last_opcode = add_opcode(last_opcode, n++, "call",    0xF800, O_MONO_OR_LIT | O_LIT_REF);
    last_opcode = add_opcode(last_opcode, n++, "cout",    0x2800, O_OPT_IMM_8);
    last_opcode = add_opcode(last_opcode, n++, "creg",    0x2900, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "cregx",   0x2910, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "crega",   0x2920, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "cin",     0x2980, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "cinx",    0x2990, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "cina",    0x29A0, O_MONO);

    // not opcodes:
    // last_opcode = add_opcode(last_opcode, n++, "repeat",  0x0000, O_ZERO);
    // last_opcode = add_opcode(last_opcode, n++, "pad",     0x0000, O_ZERO);

    last_opcode = add_opcode(last_opcode, n++, "mod.w",   0x8000, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mod.d",   0x8040, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mod",     0x8080, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mod.wr",  0x8100, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mod.dr",  0x8140, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mod.r",   0x8180, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "mod.wi",  0x80C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mod.di",  0x80D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mod.i",   0x80E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mod.wri", 0x81C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mod.dri", 0x81D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mod.ri",  0x81E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "add.w",   0x8200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "add.d",   0x8240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "add",     0x8280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "add.wr",  0x8300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "add.dr",  0x8340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "add.r",   0x8380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "add.wi",  0x82C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "add.di",  0x82D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "add.i",   0x82E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.wri", 0x83C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.dri", 0x83D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.ri",  0x83E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "lsh.w",   0x8400, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "lsh.d",   0x8440, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "lsh",     0x8480, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "lsh.wr",  0x8500, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "lsh.dr",  0x8540, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "lsh.r",   0x8580, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "lsh.wi",  0x84C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "lsh.di",  0x84D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "lsh.i",   0x84E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.wri", 0x85C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.dri", 0x85D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.ri",  0x85E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "sub.w",   0x8600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sub.d",   0x8640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sub",     0x8680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sub.wr",  0x8700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sub.dr",  0x8740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sub.r",   0x8780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "sub.wi",  0x86C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sub.di",  0x86D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sub.i",   0x86E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.wri", 0x87C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.dri", 0x87D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.ri",  0x87E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "mul.w",   0x8800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mul.d",   0x8840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mul",     0x8880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mul.wr",  0x8900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mul.dr",  0x8940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "mul.r",   0x8980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "mul.wi",  0x88C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mul.di",  0x88D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mul.i",   0x88E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.wri", 0x89C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.dri", 0x89D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.ri",  0x89E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "rsh.w",   0x8A00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rsh.d",   0x8A40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rsh",     0x8A80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rsh.wr",  0x8B00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rsh.dr",  0x8B40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rsh.r",   0x8B80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "rsh.wi",  0x8AC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "rsh.di",  0x8AD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "rsh.i",   0x8AE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.wri", 0x8BC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.dri", 0x8BD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.ri",  0x8BE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "div.w",   0x8C00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "div.d",   0x8C40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "div",     0x8C80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "div.wr",  0x8D00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "div.dr",  0x8D40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "div.r",   0x8D80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "div.wi",  0x8CC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "div.di",  0x8CD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "div.i",   0x8CE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.wri", 0x8DC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.dri", 0x8DD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.ri",  0x8DE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "cmp.w",   0x8E00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmp.d",   0x8E40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmp",     0x8E80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmp.wr",  0x8F00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmp.dr",  0x8F40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmp.r",   0x8F80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "cmp.wi",  0x8EC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cmp.di",  0x8ED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cmp.i",   0x8EE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.wri", 0x8FC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.dri", 0x8FD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.ri",  0x8FE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "smod.w",   0x9000, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smod.d",   0x9040, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smod",     0x9080, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smod.wr",  0x9100, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smod.dr",  0x9140, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smod.r",   0x9180, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "smod.wi",  0x90C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "smod.di",  0x90D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "smod.i",   0x90E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smod.wri", 0x91C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smod.dri", 0x91D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smod.ri",  0x91E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "sadd.w",   0x9200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sadd.d",   0x9240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sadd",     0x9280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sadd.wr",  0x9300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sadd.dr",  0x9340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sadd.r",   0x9380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "sadd.wi",  0x92C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sadd.di",  0x92D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sadd.i",   0x92E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sadd.wri", 0x93C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sadd.dri", 0x93D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sadd.ri",  0x93E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "slsh.w",   0x9400, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "slsh.d",   0x9440, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "slsh",     0x9480, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "slsh.wr",  0x9500, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "slsh.dr",  0x9540, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "slsh.r",   0x9580, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "slsh.wi",  0x94C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "slsh.di",  0x94D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "slsh.i",   0x94E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "slsh.wri", 0x95C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "slsh.dri", 0x95D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "slsh.ri",  0x95E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "ssub.w",   0x9600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ssub.d",   0x9640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ssub",     0x9680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ssub.wr",  0x9700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ssub.dr",  0x9740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ssub.r",   0x9780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "ssub.wi",  0x96C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "ssub.di",  0x96D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "ssub.i",   0x96E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ssub.wri", 0x97C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ssub.dri", 0x97D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ssub.ri",  0x97E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "smul.w",   0x9800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smul.d",   0x9840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smul",     0x9880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smul.wr",  0x9900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smul.dr",  0x9940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "smul.r",   0x9980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "smul.wi",  0x98C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "smul.di",  0x98D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "smul.i",   0x98E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smul.wri", 0x99C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smul.dri", 0x99D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "smul.ri",  0x99E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "srsh.w",   0x9A00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "srsh.d",   0x9A40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "srsh",     0x9A80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "srsh.wr",  0x9B00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "srsh.dr",  0x9B40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "srsh.r",   0x9B80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "srsh.wi",  0x9AC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "srsh.di",  0x9AD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "srsh.i",   0x9AE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "srsh.wri", 0x9BC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "srsh.dri", 0x9BD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "srsh.ri",  0x9BE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "sdiv.w",   0x9C00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.d",   0x9C40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sdiv",     0x9C80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.wr",  0x9D00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.dr",  0x9D40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.r",   0x9D80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "sdiv.wi",  0x9CC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.di",  0x9CD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.i",   0x9CE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.wri", 0x9DC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.dri", 0x9DD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sdiv.ri",  0x9DE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "scmp.w",   0x9E00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "scmp.d",   0x9E40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "scmp",     0x9E80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "scmp.wr",  0x9F00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "scmp.dr",  0x9F40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "scmp.r",   0x9F80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "scmp.wi",  0x9EC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "scmp.di",  0x9ED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "scmp.i",   0x9EE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "scmp.wri", 0x9FC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "scmp.dri", 0x9FD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "scmp.ri",  0x9FE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "bmod.w",   0xA000, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmod.d",   0xA040, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmod",     0xA080, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmod.wr",  0xA100, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmod.dr",  0xA140, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmod.r",   0xA180, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "bmod.wi",  0xA0C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "bmod.di",  0xA0D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "bmod.i",   0xA0E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmod.wri", 0xA1C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmod.dri", 0xA1D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmod.ri",  0xA1E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "badd.w",   0xA200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "badd.d",   0xA240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "badd",     0xA280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "badd.wr",  0xA300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "badd.dr",  0xA340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "badd.r",   0xA380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "badd.wi",  0xA2C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "badd.di",  0xA2D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "badd.i",   0xA2E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "badd.wri", 0xA3C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "badd.dri", 0xA3D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "badd.ri",  0xA3E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "bsub.w",   0xA600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bsub.d",   0xA640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bsub",     0xA680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bsub.wr",  0xA700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bsub.dr",  0xA740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bsub.r",   0xA780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "bsub.wi",  0xA6C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "bsub.di",  0xA6D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "bsub.i",   0xA6E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bsub.wri", 0xA7C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bsub.dri", 0xA7D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bsub.ri",  0xA7E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "bmul.w",   0xA800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmul.d",   0xA840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmul",     0xA880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmul.wr",  0xA900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmul.dr",  0xA940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bmul.r",   0xA980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "bmul.wi",  0xA8C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "bmul.di",  0xA8D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "bmul.i",   0xA8E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmul.wri", 0xA9C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmul.dri", 0xA9D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bmul.ri",  0xA9E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "bdiv.w",   0xAC00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.d",   0xAC40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bdiv",     0xAC80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.wr",  0xAD00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.dr",  0xAD40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.r",   0xAD80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "bdiv.wi",  0xACC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.di",  0xACD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.i",   0xACE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.wri", 0xADC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.dri", 0xADD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "bdiv.ri",  0xADE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

	last_opcode = add_opcode(last_opcode, n++, "fadd.w",   0xB200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fadd.d",   0xB240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fadd",     0xB280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fadd.wr",  0xB300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fadd.dr",  0xB340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fadd.r",   0xB380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fadd.wi",  0xB2C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fadd.di",  0xB2D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fadd.i",   0xB2E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fadd.wri", 0xB3C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fadd.dri", 0xB3D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fadd.ri",  0xB3E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "fpow.w",   0xB400, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fpow.d",   0xB440, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fpow",     0xB480, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fpow.wr",  0xB500, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fpow.dr",  0xB540, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fpow.r",   0xB580, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fpow.wi",  0xB4C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fpow.di",  0xB4D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fpow.i",   0xB4E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fpow.wri", 0xB5C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fpow.dri", 0xB5D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fpow.ri",  0xB5E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "fsub.w",   0xB600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fsub.d",   0xB640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fsub",     0xB680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fsub.wr",  0xB700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fsub.dr",  0xB740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fsub.r",   0xB780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fsub.wi",  0xB6C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fsub.di",  0xB6D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fsub.i",   0xB6E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fsub.wri", 0xB7C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fsub.dri", 0xB7D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fsub.ri",  0xB7E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "fmul.w",   0xB800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fmul.d",   0xB840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fmul",     0xB880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fmul.wr",  0xB900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fmul.dr",  0xB940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fmul.r",   0xB980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fmul.wi",  0xB8C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fmul.di",  0xB8D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fmul.i",   0xB8E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fmul.wri", 0xB9C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fmul.dri", 0xB9D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fmul.ri",  0xB9E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "flog.w",   0xBA00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "flog.d",   0xBA40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "flog",     0xBA80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "flog.wr",  0xBB00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "flog.dr",  0xBB40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "flog.r",   0xBB80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "flog.wi",  0xBAC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "flog.di",  0xBAD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "flog.i",   0xBAE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "flog.wri", 0xBBC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "flog.dri", 0xBBD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "flog.ri",  0xBBE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "fdiv.w",   0xBC00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.d",   0xBC40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fdiv",     0xBC80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.wr",  0xBD00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.dr",  0xBD40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.r",   0xBD80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fdiv.wi",  0xBCC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.di",  0xBCD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.i",   0xBCE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.wri", 0xBDC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.dri", 0xBDD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fdiv.ri",  0xBDE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "fcmp.w",   0xBE00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.d",   0xBE40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fcmp",     0xBE80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.wr",  0xBF00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.dr",  0xBF40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.r",   0xBF80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "fcmp.wi",  0xBEC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.di",  0xBED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.i",   0xBEE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.wri", 0xBFC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.dri", 0xBFD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "fcmp.ri",  0xBFE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

	last_opcode = add_opcode(last_opcode, n++, "not.w",   0xC200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "not.d",   0xC240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "not",     0xC280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "not.wr",  0xC300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "not.dr",  0xC340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "not.r",   0xC380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "not.wi",  0xC2C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "not.di",  0xC2D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "not.i",   0xC2E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "not.wri", 0xC3C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "not.dri", 0xC3D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "not.ri",  0xC3E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "xor.w",   0xC400, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "xor.d",   0xC440, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "xor",     0xC480, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "xor.wr",  0xC500, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "xor.dr",  0xC540, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "xor.r",   0xC580, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "xor.wi",  0xC4C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "xor.di",  0xC4D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "xor.i",   0xC4E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.wri", 0xC5C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.dri", 0xC5D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.ri",  0xC5E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "ror.w",   0xC600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ror.d",   0xC640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ror",     0xC680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ror.wr",  0xC700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ror.dr",  0xC740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ror.r",   0xC780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "ror.wi",  0xC6C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "ror.di",  0xC6D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "ror.i",   0xC6E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.wri", 0xC7C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.dri", 0xC7D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.ri",  0xC7E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "and.w",   0xC800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "and.d",   0xC840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "and",     0xC880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "and.wr",  0xC900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "and.dr",  0xC940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "and.r",   0xC980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "and.wi",  0xC8C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "and.di",  0xC8D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "and.i",   0xC8E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "and.wri", 0xC9C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "and.dri", 0xC9D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "and.ri",  0xC9E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "or.w",   0xCA00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "or.d",   0xCA40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "or",     0xCA80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "or.wr",  0xCB00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "or.dr",  0xCB40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "or.r",   0xCB80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "or.wi",  0xCAC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "or.di",  0xCAD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "or.i",   0xCAE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.wri", 0xCBC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.dri", 0xCBD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.ri",  0xCBE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "rol.w",   0xCC00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rol.d",   0xCC40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rol",     0xCC80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rol.wr",  0xCD00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rol.dr",  0xCD40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "rol.r",   0xCD80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "rol.wi",  0xCCC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "rol.di",  0xCCD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "rol.i",   0xCCE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.wri", 0xCDC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.dri", 0xCDD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.ri",  0xCDE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "popcount.w",   0xCE00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "popcount.d",   0xCE40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "popcount",     0xCE80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "popcount.wr",  0xCF00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "popcount.dr",  0xCF40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "popcount.r",   0xCF80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "popcount.wi",  0xCEC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "popcount.di",  0xCED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "popcount.i",   0xCEE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "popcount.wri", 0xCFC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "popcount.dri", 0xCFD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "popcount.ri",  0xCFE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

	last_opcode = add_opcode(last_opcode, n++, "cadd.w",   0xD200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cadd.d",   0xD240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cadd",     0xD280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cadd.wr",  0xD300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cadd.dr",  0xD340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cadd.r",   0xD380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "cadd.wi",  0xD2C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cadd.di",  0xD2D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cadd.i",   0xD2E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cadd.wri", 0xD3C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cadd.dri", 0xD3D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cadd.ri",  0xD3E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "clsh.w",   0xD400, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "clsh.d",   0xD440, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "clsh",     0xD480, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "clsh.wr",  0xD500, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "clsh.dr",  0xD540, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "clsh.r",   0xD580, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "clsh.wi",  0xD4C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "clsh.di",  0xD4D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "clsh.i",   0xD4E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "clsh.wri", 0xD5C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "clsh.dri", 0xD5D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "clsh.ri",  0xD5E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "csub.w",   0xD600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "csub.d",   0xD640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "csub",     0xD680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "csub.wr",  0xD700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "csub.dr",  0xD740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "csub.r",   0xD780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "csub.wi",  0xD6C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "csub.di",  0xD6D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "csub.i",   0xD6E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "csub.wri", 0xD7C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "csub.dri", 0xD7D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "csub.ri",  0xD7E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "cmul.w",   0xD800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmul.d",   0xD840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmul",     0xD880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmul.wr",  0xD900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmul.dr",  0xD940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cmul.r",   0xD980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "cmul.wi",  0xD8C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cmul.di",  0xD8D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cmul.i",   0xD8E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmul.wri", 0xD9C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmul.dri", 0xD9D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmul.ri",  0xD9E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "crsh.w",   0xDA00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "crsh.d",   0xDA40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "crsh",     0xDA80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "crsh.wr",  0xDB00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "crsh.dr",  0xDB40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "crsh.r",   0xDB80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "crsh.wi",  0xDAC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "crsh.di",  0xDAD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "crsh.i",   0xDAE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "crsh.wri", 0xDBC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "crsh.dri", 0xDBD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "crsh.ri",  0xDBE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "cdiv.w",   0xDC00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.d",   0xDC40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cdiv",     0xDC80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.wr",  0xDD00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.dr",  0xDD40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.r",   0xDD80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "cdiv.wi",  0xDCC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.di",  0xDCD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.i",   0xDCE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.wri", 0xDDC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.dri", 0xDDD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cdiv.ri",  0xDDE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "ccmp.w",   0xDE00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.d",   0xDE40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp",     0xDE80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.wr",  0xDF00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.dr",  0xDF40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.r",   0xDF80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "ccmp.wi",  0xDEC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.di",  0xDED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.i",   0xDEE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.wri", 0xDFC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.dri", 0xDFD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.ri",  0xDFE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
	
    last_opcode = add_opcode(last_opcode, n++, "ccmp.we",  0xD000, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.de",  0xD040, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ccmp.e",   0xD080, O_ARITHMETIC_DUO);
	
    last_opcode = add_opcode(last_opcode, n++, "dadd.w",   0xE200, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dadd.d",   0xE240, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dadd",     0xE280, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dadd.wr",  0xE300, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dadd.dr",  0xE340, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dadd.r",   0xE380, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "dadd.wi",  0xE2C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "dadd.di",  0xE2D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "dadd.i",   0xE2E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dadd.wri", 0xE3C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dadd.dri", 0xE3D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dadd.ri",  0xE3E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "dsub.w",   0xE600, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dsub.d",   0xE640, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dsub",     0xE680, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dsub.wr",  0xE700, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dsub.dr",  0xE740, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dsub.r",   0xE780, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "dsub.wi",  0xE6C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "dsub.di",  0xE6D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "dsub.i",   0xE6E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dsub.wri", 0xE7C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dsub.dri", 0xE7D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dsub.ri",  0xE7E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "dmul.w",   0xE800, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dmul.d",   0xE840, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dmul",     0xE880, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dmul.wr",  0xE900, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dmul.dr",  0xE940, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dmul.r",   0xE980, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "dmul.wi",  0xE8C8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "dmul.di",  0xE8D0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "dmul.i",   0xE8E0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dmul.wri", 0xE9C8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dmul.dri", 0xE9D0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dmul.ri",  0xE9E0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "ddiv.w",   0xEC00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.d",   0xEC40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ddiv",     0xEC80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.wr",  0xED00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.dr",  0xED40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.r",   0xED80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "ddiv.wi",  0xECC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.di",  0xECD0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.i",   0xECE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.wri", 0xEDC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.dri", 0xEDD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ddiv.ri",  0xEDE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "dcmp.w",   0xEE00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.d",   0xEE40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dcmp",     0xEE80, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.wr",  0xEF00, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.dr",  0xEF40, O_ARITHMETIC_DUO);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.r",   0xEF80, O_ARITHMETIC_DUO);

    last_opcode = add_opcode(last_opcode, n++, "dcmp.wi",  0xEEC8, O_ARITHMETIC_LIT | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.di",  0xEED0, O_ARITHMETIC_LIT | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.i",   0xEEE0, O_ARITHMETIC_LIT | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.wri", 0xEFC8, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.dri", 0xEFD0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "dcmp.ri",  0xEFE0, O_ARITHMETIC_LIT | O_LIT_REF | O_LIT_64);

    /*
    // original design (QQQQQRRRR) arithmetic opcodes:
    // unsigned
    // cmp
    last_opcode = add_opcode(last_opcode, n++, "cmp.u",   0x8000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp",     0x8000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp.uw",  0x8110, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cmp.ud",  0x8120, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cmp.uq",  0x8140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.ur",  0x81C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.r",   0x81C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.uqr", 0x81C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.udr", 0x81A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.uwr", 0x8190, O_MONO | O_LIT_64);
    // add
    last_opcode = add_opcode(last_opcode, n++, "add.u",   0x8200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add",     0x8200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.uw",  0x8310, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "add.ud",  0x8320, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "add.uq",  0x8340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.ur",  0x83C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.r",   0x83C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.uqr", 0x83C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.udr", 0x83A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.uwr", 0x8390, O_MONO | O_LIT_64);
    // sub
    last_opcode = add_opcode(last_opcode, n++, "sub.u",   0x8600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub",     0x8600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.uw",  0x8710, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sub.ud",  0x8720, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sub.uq",  0x8740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.ur",  0x87C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.r",   0x87C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.uqr", 0x87C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.udr", 0x87A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.uwr", 0x8790, O_MONO | O_LIT_64);
    // mul
    last_opcode = add_opcode(last_opcode, n++, "mul.u",   0x8A00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul",     0x8A00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.uw",  0x8B10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mul.ud",  0x8B20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mul.uq",  0x8B40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.ur",  0x8BC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.r",   0x8BC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.uqr", 0x8BC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.udr", 0x8BA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.uwr", 0x8B90, O_MONO | O_LIT_64);
    // div
    last_opcode = add_opcode(last_opcode, n++, "div.u",   0x8E00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div",     0x8E00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.uw",  0x8F10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "div.ud",  0x8F20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "div.uq",  0x8F40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.ur",  0x8FC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.r",   0x8FC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.uqr", 0x8FC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.udr", 0x8FA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.uwr", 0x8F90, O_MONO | O_LIT_64);

    // signed
    // cmp
    last_opcode = add_opcode(last_opcode, n++, "cmp.s",   0x9000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sw",  0x9110, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sd",  0x9120, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sq",  0x9140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sr",  0x91C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sqr", 0x91C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.sdr", 0x91A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.swr", 0x9190, O_MONO | O_LIT_64);
    // add
    last_opcode = add_opcode(last_opcode, n++, "add.s",   0x9200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.sw",  0x9310, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "add.sd",  0x9320, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "add.sq",  0x9340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.sr",  0x93C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.sqr", 0x93C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.sdr", 0x93A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.swr", 0x9390, O_MONO | O_LIT_64);
    // sub
    last_opcode = add_opcode(last_opcode, n++, "sub.s",   0x9600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.sw",  0x9710, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sub.sd",  0x9720, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sub.sq",  0x9740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.sr",  0x97C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.sqr", 0x97C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.sdr", 0x97A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.swr", 0x9790, O_MONO | O_LIT_64);
    // mul
    last_opcode = add_opcode(last_opcode, n++, "mul.s",   0x9A00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.sw",  0x9B10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mul.sd",  0x9B20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mul.sq",  0x9B40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.sr",  0x9BC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.sqr", 0x9BC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.sdr", 0x9BA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.swr", 0x9B90, O_MONO | O_LIT_64);
    // div
    last_opcode = add_opcode(last_opcode, n++, "div.s",   0x9E00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.sw",  0x9F10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "div.sd",  0x9F20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "div.sq",  0x9F40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.sr",  0x9FC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.sqr", 0x9FC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.sdr", 0x9FA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.swr", 0x9F90, O_MONO | O_LIT_64);

    // sign-extend left shift (signed integers only)
    last_opcode = add_opcode(last_opcode, n++, "lsh.s",   0x9400, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sw",  0x9510, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sd",  0x9520, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sq",  0x9540, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sr",  0x95C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sqr", 0x95C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.sdr", 0x95A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.swr", 0x9590, O_MONO | O_LIT_64);
    // sign-extend right shift (signed integers only)
    last_opcode = add_opcode(last_opcode, n++, "rsh.s",   0x9800, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sw",  0x9910, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sd",  0x9920, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sq",  0x9940, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sr",  0x99C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sqr", 0x99C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.sdr", 0x99A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.swr", 0x9990, O_MONO | O_LIT_64);

    // multi-bytes
    // add
    last_opcode = add_opcode(last_opcode, n++, "add.b",   0xA200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.bw",  0xA310, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "add.bd",  0xA320, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "add.bq",  0xA340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.br",  0xA3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.bqr", 0xA3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.bdr", 0xA3A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.bwr", 0xA390, O_MONO | O_LIT_64);
    // sub
    last_opcode = add_opcode(last_opcode, n++, "sub.b",   0xA600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.bw",  0xA710, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sub.bd",  0xA720, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sub.bq",  0xA740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.br",  0xA7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.bqr", 0xA7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.bdr", 0xA7A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.bwr", 0xA790, O_MONO | O_LIT_64);
    // mul
    last_opcode = add_opcode(last_opcode, n++, "mul.b",   0xAA00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.bw",  0xAB10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mul.bd",  0xAB20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mul.bq",  0xAB40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.br",  0xABC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.bqr", 0xABC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.bdr", 0xABA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.bwr", 0xAB90, O_MONO | O_LIT_64);
    // div
    last_opcode = add_opcode(last_opcode, n++, "div.b",   0xAE00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.bw",  0xAF10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "div.bd",  0xAF20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "div.bq",  0xAF40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.br",  0xAFC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.bqr", 0xAFC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.bdr", 0xAFA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.bwr", 0xAF90, O_MONO | O_LIT_64);

    // float
    // cmp
    last_opcode = add_opcode(last_opcode, n++, "cmp.f",   0xB000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fw",  0xB110, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fd",  0xB120, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fq",  0xB140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fr",  0xB1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fqr", 0xB1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fdr", 0xB1A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.fwr", 0xB190, O_MONO | O_LIT_64);
    // add
    last_opcode = add_opcode(last_opcode, n++, "add.f",   0xB200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.fw",  0xB310, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "add.fd",  0xB320, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "add.fq",  0xB340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.fr",  0xB3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.fqr", 0xB3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.fdr", 0xB3A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.fwr", 0xB390, O_MONO | O_LIT_64);
    // sub
    last_opcode = add_opcode(last_opcode, n++, "sub.f",   0xB600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.fw",  0xB710, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "sub.fd",  0xB720, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "sub.fq",  0xB740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.fr",  0xB7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.fqr", 0xB7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.fdr", 0xB7A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.fwr", 0xB790, O_MONO | O_LIT_64);
    // mul
    last_opcode = add_opcode(last_opcode, n++, "mul.f",   0xBA00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.fw",  0xBB10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "mul.fd",  0xBB20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "mul.fq",  0xBB40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.fr",  0xBBC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.fqr", 0xBBC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.fdr", 0xBBA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.fwr", 0xBB90, O_MONO | O_LIT_64);
    // div
    last_opcode = add_opcode(last_opcode, n++, "div.f",   0xBE00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.fw",  0xBF10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "div.fd",  0xBF20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "div.fq",  0xBF40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.fr",  0xBFC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.fqr", 0xBFC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.fdr", 0xBFA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.fwr", 0xBF90, O_MONO | O_LIT_64);

    // bitwise
    last_opcode = add_opcode(last_opcode, n++, "not",     0xC000, O_MONO);
    last_opcode = add_opcode(last_opcode, n++, "store.q", 0xC040, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "store",   0xC040, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "store.d", 0xC020, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "store.w", 0xC010, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "store.qr",0xC0C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "store.r", 0xC0C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "store.dr",0xC0A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "store.wr",0xC090, O_MONO | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "load.q",  0xC140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "load",    0xC140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "load.d",  0xC120, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "load.w",  0xC110, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "load.r",  0xC1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "load.qr", 0xC1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "load.dr", 0xC1A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "load.wr", 0xC190, O_MONO | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "and",     0xC800, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "or",      0xCC00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "xor",     0xC400, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "lsh",     0xC200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "rsh",     0xC600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "rol",     0xCA00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "ror",     0xCE00, O_MONO | O_Q);

    last_opcode = add_opcode(last_opcode, n++, "and.q",   0xC840, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.q",    0xCC40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.q",   0xC440, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.q",   0xC240, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.q",   0xC640, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.q",   0xCA40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.q",   0xCE40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "and.qr",  0xC8C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.qr",   0xCCC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.qr",  0xC4C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.qr",  0xC2C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.qr",  0xC6C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.qr",  0xCAC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.qr",  0xCEC0, O_MONO | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "and.d",   0xC820, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "or.d",    0xCC20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "xor.d",   0xC420, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "lsh.d",   0xC220, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "rsh.d",   0xC620, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "rol.d",   0xCA20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "ror.d",   0xCE20, O_MONO | O_LIT_32);
    last_opcode = add_opcode(last_opcode, n++, "and.dr",  0xC8A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.dr",   0xCCA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.dr",  0xC4A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.dr",  0xC2A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.dr",  0xC6A0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.dr",  0xCAA0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.dr",  0xCEA0, O_MONO | O_LIT_64);

    last_opcode = add_opcode(last_opcode, n++, "and.w",   0xC810, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "or.w",    0xCC10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "xor.w",   0xC410, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "lsh.w",   0xC210, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "rsh.w",   0xC610, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "rol.w",   0xCA10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "ror.w",   0xCE10, O_MONO | O_LIT_16);
    last_opcode = add_opcode(last_opcode, n++, "and.wr",  0xC890, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "or.wr",   0xCC90, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "xor.wr",  0xC490, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "lsh.wr",  0xC290, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rsh.wr",  0xC690, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "rol.wr",  0xCA90, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "ror.wr",  0xCE90, O_MONO | O_LIT_64);

    // CHASMv2 CONS.d
    // general
    last_opcode = add_opcode(last_opcode, n++, "add.c",   0xD200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.c",   0xD600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.c",   0xDA00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.c",   0xDE00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.cq",  0xD340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.cq",  0xD740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.cq",  0xDB40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.cq",  0xDF40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.cqr", 0xD3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.cqr", 0xD7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.cqr", 0xDBC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.cqr", 0xDFC0, O_MONO | O_LIT_64);
    
    // cmp
    last_opcode = add_opcode(last_opcode, n++, "cmp.c",   0xD000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp.cq",  0xD140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.cr",  0xD1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.cqr", 0xD1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.ce",  0xD800, O_MONO | O_Q);

    // CHASMv2 BCD
    // general
    last_opcode = add_opcode(last_opcode, n++, "add.d",   0xE200, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "sub.d",   0xE600, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "mul.d",   0xEA00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "div.d",   0xEE00, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "add.dq",  0xE340, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.dq",  0xE740, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.dq",  0xEB40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.dq",  0xEF40, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "add.dqr", 0xE3C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "sub.dqr", 0xE7C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "mul.dqr", 0xEBC0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "div.dqr", 0xEFC0, O_MONO | O_LIT_64);

    // cmp
    last_opcode = add_opcode(last_opcode, n++, "cmp.d",   0xE000, O_MONO | O_Q);
    last_opcode = add_opcode(last_opcode, n++, "cmp.dq",  0xE140, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.dr",  0xE1C0, O_MONO | O_LIT_64);
    last_opcode = add_opcode(last_opcode, n++, "cmp.dqr", 0xE1C0, O_MONO | O_LIT_64);
    */

    printf(" -- loaded %i opcodes\n", n);
}