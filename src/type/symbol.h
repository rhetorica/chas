
#ifndef SYMBOL_H
    #define SYMBOL_H
    #include "../main.hpp"

    struct label {
        const char* text;
        unsigned long long address;
        token* token;
    };

    extern link<label>* labels;
    int collect_labels(link<token>*);
    int symbolize(link<token>*);
    int apply_patches();
    int write_emissions(char*);
#endif // SYMBOL_H
