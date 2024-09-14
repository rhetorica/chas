#include "../main.hpp"
#include "register.h"

link<reg>* regs = NULL;

reg* find_register(char* text) {
    link<reg>* r = regs;
    while(r != NULL) {
        if(strcmp(text, r->child->name) == 0)
            return r->child;
        r = r->next;
    }
    return NULL;
}

void init_registers() {
    link<reg>* last_reg = NULL;
    for(int i = 0; i < REGISTER_COUNT; ++i) {
        link<reg>* new_reg = new link<reg>();
        new_reg->child = new reg();
        new_reg->child->number = i;
        switch(i) {
            case  0: new_reg->child->name = "a";      break;
            case  1: new_reg->child->name = "b";      break;
            case  2: new_reg->child->name = "c";      break;
            case  3: new_reg->child->name = "d";      break;
            case  4: new_reg->child->name = "e";      break;
            case  5: new_reg->child->name = "f";      break;
            case  6: new_reg->child->name = "g";      break;
            case  7: new_reg->child->name = "h";      break;
            case  8: new_reg->child->name = "pc";     break;
            case  9: new_reg->child->name = "ra";     break;
            case 10: new_reg->child->name = "sb";     break;
            case 11: new_reg->child->name = "sp";     break;
            case 12: new_reg->child->name = "db";     break;
            case 13: new_reg->child->name = "cb";     break;
            case 14: new_reg->child->name = "pi";     break;
            case 15: new_reg->child->name = "status"; break;
        }
        if(last_reg != NULL) {
            last_reg->next = new_reg;
        } else {
            regs = new_reg;
        }
        last_reg = new_reg;
        printf(" -- register %i = %s\n", new_reg->child->number, new_reg->child->name);
    }
    last_reg->next = NULL;
    printf(" -- initialized %i registers\n", REGISTER_COUNT);
}

