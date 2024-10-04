
#include "symbol.h"
#include "opcode.h"

link<label>* labels = NULL;

struct patch_address {
    token* instruction;
    label* label;
};

struct emission {
    token* origin;
    // size in bytes:
    fpos_t size;
    char* text;
    fpos_t repeats = 0; // additional bytes to append, not counting size
};

struct definition {
    token* replacement;
    char* text;
};

link<patch_address>* unpatched = NULL;
link<emission>* emissions = NULL;
link<emission>* last_emission = NULL;
link<definition>* definitions = NULL;

int gather_definitions(link<token>* t) {
    printf(" -- gathering definitions...\n");
    link<definition>* last_definition;

    // line number in current file:
    fpos_t ln = 0;
    // current file:
    char* filename;
    while(t != NULL) {
        while(t->child->line == 0) {
            t = t->next;
        }
        fpos_t new_ln = t->child->line;
        if(new_ln != ln || filename != t->child->filename) {
            ln = new_ln;
            filename = t->child->filename;
            if(t->child->type == TK_NAME && strcmp(t->child->text, "define") == 0) {
                link<definition>* d = new link<definition>;
                if(t->next == NULL) {
                    fprintf(stderr, " -- truncated definition on line %lli of %s\n", ln, filename);
                    return 1;
                }

                if(t->next->next == NULL) {
                    fprintf(stderr, " -- truncated definition on line %lli of %s\n", ln, filename);
                    return 1;
                }

                if(t->next->child->line != ln || t->next->child->filename != filename) {
                    fprintf(stderr, " -- truncated definition on line %lli of %s\n", ln, filename);
                    return 1;
                }

                if(t->next->next->child->line != ln || t->next->next->child->filename != filename) {
                    fprintf(stderr, " -- truncated definition on line %lli of %s\n", ln, filename);
                    return 1;
                }

                printf(" -- found definition %s='%s' on line %lli of %s\n",
                    t->next->child->text,
                    t->next->next->child->text,
                    t->child->line,
                    t->child->filename
                );

                t->next->child->type = TK_DEFINITION;
                d->child = new definition;
                d->child->text = t->next->child->text;
                d->child->replacement = t->next->next->child;

                if(last_definition != NULL)
                    last_definition->next = d;
                
                last_definition = d;
                
                if(definitions == NULL)
                    definitions = d;
            }
        }
        t = t->next;
    }

    return 0;
}

int apply_definitions(link<token>* t) {
    printf(" -- applying definitions...\n");

    while(t != NULL) {
        while(t->child->line == 0) {
            t = t->next;
        }
        
        if(t->child->type == TK_NAME) {
            link<definition>* d = definitions;
            while(d != NULL) {
                if(strcmp(t->child->text, d->child->text) == 0) {
                    printf(" -- substituting '%s' on line %lli of %s\n", t->child->text, t->child->line, t->child->filename);
                    
                    free(t->child->text);
                    t->child->text = d->child->replacement->text;
                    t->child->type = d->child->replacement->type;
                    t->child->literal_numeric_value = d->child->replacement->literal_numeric_value;
                    t->child->reg = d->child->replacement->reg;
                    t->child->output = d->child->replacement->output;
                    t->child->output_size = d->child->replacement->output_size;
                    
                    // other attributes haven't been added yet
                }
                d = d->next;
            }
        }
        t = t->next;
    }
    return 0;
}

int write_emissions(char* filename) {
    printf(" -- writing output...\n");
    FILE* f = fopen(filename, "wb");
    if(f == NULL) {
        fprintf(stderr, " -- could not open output file: %s\n", filename);
        return 1;
    } else {
        link<emission>* e = emissions;
        while(e != NULL) {
            fwrite(e->child->text, sizeof(char), e->child->size, f);
            if(e->child->repeats > 0) {
                fpos_t r = e->child->repeats;
                while(r >= e->child->size) {
                    fwrite(e->child->text, sizeof(char), e->child->size, f);
                    r -= e->child->size;
                }
                if(r > 0) {
                    fwrite(e->child->text, sizeof(char), r, f);
                }
            }
            e = e->next;
        }
        fclose(f);
    }
    return 0;
}

int apply_patches() {
    printf(" -- applying patches...\n");
    link<patch_address>* p = unpatched;
    while(p != NULL) {
        unsigned long long address = p->child->label->address;
        if(p->child->instruction->output_size != 10) {
            fprintf(stderr, " -- internal error: instruction on line %lli of %s did not allocate space for patching address literal\n", p->child->instruction->line, p->child->instruction->filename);
            return 1;
        } else {
            p->child->instruction->output[2] = (address >> 56) & 0xff;
            p->child->instruction->output[3] = (address >> 48) & 0xff;
            p->child->instruction->output[4] = (address >> 40) & 0xff;
            p->child->instruction->output[5] = (address >> 32) & 0xff;
            p->child->instruction->output[6] = (address >> 24) & 0xff;
            p->child->instruction->output[7] = (address >> 16) & 0xff;
            p->child->instruction->output[8] = (address >>  8) & 0xff;
            p->child->instruction->output[9] = (address >>  0) & 0xff;
            printf(" -- patched address to '%s' (0x%llx) on line %lli of %s\n", p->child->label->text, p->child->label->address, p->child->instruction->line, p->child->instruction->filename);
        }
        p = p->next;
    }
    p = unpatched;
    while(p != NULL) {
        link<patch_address>* pnext = p->next;
        delete p->child;
        delete p;
        p = pnext;
    }
    return 0;
}

int collect_labels(link<token>* t) {
    printf(" -- collecting labels...\n");
    link<label>* last_label = labels = new link<label>;
    labels->child = new label;
    labels->child->text = "start";
    labels->child->address = base_address;
    labels->child->token = t->child;
    labels->next = NULL;
    
    t = t->next;

    int label_count = 0;
    while(t != NULL) {
        if(t->child->type == TK_LABEL) {
            printf(" -- found label '%s' on line %lli\n", last_label->child->text, t->child->line);
            link<label>* s = labels;
            while(s != NULL) {
                if(strcmp(s->child->text, t->child->text) == 0) {
                    fprintf(stderr, " -- label '%s' on line %lli of %s redefined on line %lli of %s\n", s->child->text, s->child->token->line, s->child->token->filename, t->child->line, t->child->filename);
                    return 1;
                }
                s = s->next;
            }
            ++label_count;
            last_label = last_label->next = new link<label>;
            last_label->child = new label;
            last_label->child->text = t->child->text;
            last_label->child->address = (unsigned long long)-1;
            last_label->child->token = t->child;
            last_label->next = NULL;
            t->child->label = last_label->child;
        }
        t = t->next;
    }
    printf(" -- found %i label(s) total\n", label_count);
    return 0;
}

int symbolize(link<token>* t) {
    printf(" -- compiling...\n");

    // line number in current file:
    fpos_t ln = 0;
    // position in words:
    fpos_t address = 0;
    // current file:
    char* filename;
    while(t != NULL) {
        while(t->child->line == 0)
            t = t->next;
        while(t->child->type == TK_LABEL) {
            t->child->address = address + base_address;
            t->child->label->address = t->child->address;
            printf("assigned address 0x%016llx to label '%s'\n", t->child->address, t->child->text);
            t = t->next;
        }
        fpos_t new_ln = t->child->line;
        if(new_ln != ln || filename != t->child->filename) {
            printf("processing line %lli\n", new_ln);
            ln = new_ln;
            filename = t->child->filename;
            printf("line %lli of %s: %s\n", ln, filename, t->child->text);
            if(t->child->type == TK_NAME && strcmp(t->child->text, "define") == 0) {
                // skip:
                t = t->next->next;
            } else if(t->child->type == TK_NAME && strcmp(t->child->text, "embed") == 0) {
                if(t->next->child->type == TK_LIT_STRING) {
                    char* embed_filename = t->next->child->text;
                    link<emission>* new_emission = new link<emission>;
                    new_emission->child = new emission;
                    new_emission->child->origin = t->child;
                    fpos_t size;
                    new_emission->child->text = get_binary_file_contents(embed_filename, size);
                    new_emission->child->size = size;
                    if(new_emission->child->size % 2)
                        new_emission->child->size += 1; // add the string zero terminator
                    if(new_emission->child->size == 0) {
                        fprintf(stderr, " -- could not embed file %s into %s at line %lli: no content\n", embed_filename, filename, ln);
                        return 1;
                    } else {
                        printf("embedded %s (%lli bytes) at address 0x%016llx\n", embed_filename, new_emission->child->size, address);
                        if(last_emission != NULL)
                            last_emission->next = new_emission;
                        else
                            emissions = new_emission;
                        
                        new_emission->next = NULL;
                        last_emission = new_emission;

                        t = t->next;
                    }
                    address += (new_emission->child->size >> 1);
                } else {
                    fprintf(stderr, " -- embed with no filename on line %lli of %s\n", ln, filename);
                    return 1;
                }
            } else if(t->child->type == TK_NAME) {
                opcode* o = find_opcode(t->child->text);
                if(o != NULL) {
                    link<token>* ins = t;
                    ins->child->op = o;
                    ins->child->address = address + base_address;
                    size_t TRAILER_SIZE = 0;
                    unsigned short opcode_emission = o->emission;

                    if(ins->child->op->format & O_MONO) {
                        link<token>* r0 = t->next;
                        if(r0->child->line != ln || r0->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r0->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                            return 1;
                        } else {
                            printf("got first register argument %s (r%02x)\n", r0->child->text, r0->child->reg->number);
                            opcode_emission = (opcode_emission & 0xfff0) | (r0->child->reg->number & 0xf);
                            ins->child->arg_0 = r0->child;
                            t = t->next;
                        }
                    } // O_MONO

                    if(ins->child->op->format & O_DUO_ONLY) {
                        link<token>* r0 = t->next;
                        if(r0->child->line != ln || r0->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r0->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                            return 1;
                        } else {
                            printf("got duo-only register argument %s (r%02x)\n", r0->child->text, r0->child->reg->number);
                            opcode_emission = (opcode_emission & 0xff0f) | ((r0->child->reg->number & 0xf) << 4);
                            ins->child->arg_0 = r0->child;
                            t = t->next;
                        }
                    } // O_DUO_ONLY

                    if(ins->child->op->format & O_MONO_OR_LIT) {
                        link<token>* r0 = t->next;
                        if(r0->child->line != ln || r0->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r0->child->type == TK_REGNAME) {
                            printf("got first register argument %s (r%02x)\n", r0->child->text, r0->child->reg->number);
                            opcode_emission = (opcode_emission & 0xfff0) | (r0->child->reg->number & 0xf);
                            ins->child->arg_0 = r0->child;
                            t = t->next;
                        } else if(r0->child->type == TK_LABEL_USAGE) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_REF)) {
                                printf("got label argument %s=%llu\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered label usage %s on line %lli of %s but opcode does not expect reference literal\n", r0->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r0->child->type == TK_LIT_QWORD) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                printf("got 64-bit literal argument %s=%llu\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered QWORD literal %s on line %lli of %s but opcode cannot accept 64-bit literal\n", r0->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r0->child->type == TK_LIT_DWORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 32-bit literal argument %s=%llu to 64-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("got 32-bit literal argument %s=%llu\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered DWORD literal %s on line %lli of %s but opcode cannot accept 32-bit literal\n", r0->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r0->child->type == TK_LIT_WORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 16-bit literal argument %s=%llu to 64-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 16-bit literal argument %s=%llu to 32-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("got 16-bit literal argument %s=%llu\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered WORD literal %s on line %lli of %s but opcode cannot accept 16-bit literal\n", r0->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r0->child->type == TK_LIT_BYTE) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 8-bit literal argument %s=%llu to 64-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 8-bit literal argument %s=%llu to 32-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("upsizing 8-bit literal argument %s=%llu to 16-bit\n", r0->child->text, r0->child->literal_numeric_value);
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                                ins->child->literal = r0->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered byte literal %s on line %lli of %s but opcode cannot accept 8-bit literal\n", r0->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r0->child->type == TK_NAME) {
                            link<label>* s = labels;
                            link<label>* mylabel = NULL;
                            while(s != NULL) {
                                if(strcmp(s->child->text, r0->child->text) == 0) {
                                    fprintf(stderr, " -- label '%s' with address %016llx on line %lli of %s used on line %lli of %s\n", s->child->text, s->child->address, s->child->token->line, s->child->token->filename, ln, filename);
                                    mylabel = s;
                                    break;
                                }
                                s = s->next;
                            }

                            if(mylabel == NULL) {
                                fprintf(stderr, " -- undefined name '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                                return 1;
                            } else {
                                ins->child->literal = r0->child;
                                r0->child->label = mylabel->child;
                                r0->child->literal_numeric_value = r0->child->label->address;
                                if(mylabel->child->address == (unsigned long long)-1) {
                                    printf(" -- unpatched address; adding to patch queue\n");
                                    link<patch_address>* new_patch = new link<patch_address>;
                                    new_patch->child = new patch_address;
                                    new_patch->child->instruction = ins->child;
                                    new_patch->child->label = mylabel->child;
                                    new_patch->next = unpatched;
                                    unpatched = new_patch;
                                }
                                TRAILER_SIZE = 4;
                                opcode_emission = (opcode_emission & 0xfff0) | 0xf;
                            }
                        } else {
                            fprintf(stderr, " -- expected register name, label usage, or numeric literal but got '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                            return 1;
                        }
                    } // O_MONO_OR_LIT

                    if(ins->child->op->format & O_FLAG) {
                        link<token>* bitflag = t->next;
                        if(bitflag->child->line != ln || bitflag->child->filename != filename) {
                            fprintf(stderr, " -- expected literal byte but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(bitflag->child->type != TK_LIT_BYTE) {
                            fprintf(stderr, " -- expected literal byte but got '%s' on line %lli of %s\n", bitflag->child->text, ln, filename);
                            return 1;
                        } else if(bitflag->child->literal_numeric_value > 63) {
                            fprintf(stderr, " -- index out of range: bit '%s' (%llu) must be in range 0-63 on line %lli of %s\n", bitflag->child->text, bitflag->child->literal_numeric_value, ln, filename);
                            return 1;
                        } else {
                            printf("got bit index (literal byte) argument %s %llu\n", bitflag->child->text, bitflag->child->literal_numeric_value);
                            opcode_emission = (opcode_emission & 0xfc0f) | ((bitflag->child->literal_numeric_value & 0x3f) << 4);
                            t = t->next;
                        }
                    } // O_FLAG

                    if(ins->child->op->format & O_DUO) {
                        link<token>* r1 = t->next;
                        if(r1->child->line != ln || r1->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r1->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r1->child->text, ln, filename);
                            return 1;
                        } else {
                            printf("got second register argument %s (r%02x)\n", r1->child->text, r1->child->reg->number);
                            opcode_emission = (opcode_emission & 0xff0f) | ((r1->child->reg->number & 0xf) << 4);
                            ins->child->arg_1 = r1->child;
                            t = t->next;
                        }
                    } // O_DUO
                    
                    if(ins->child->op->format & O_OPT_IMM_8) {
                        link<token>* byte = t->next;
                        if(byte->child->line != ln || byte->child->filename != filename) {
                            fprintf(stderr, " -- expected literal byte but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(byte->child->type == TK_LIT_STRING && strlen(byte->child->text) == 1) {
                            printf("got single character argument %s (%i)\n", byte->child->text, byte->child->text[0]);
                            opcode_emission = (opcode_emission & 0xff00) | (byte->child->text[0]);
                            t = t->next;
                        } else if(byte->child->type != TK_LIT_BYTE) {
                            fprintf(stderr, " -- expected literal byte but got '%s' on line %lli of %s\n", byte->child->text, ln, filename);
                            return 1;
                        } else {
                            printf("got literal byte argument %s %llu\n", byte->child->text, byte->child->literal_numeric_value);
                            opcode_emission = (opcode_emission & 0xff00) | (byte->child->literal_numeric_value);
                            t = t->next;
                        }
                    } // O_OPT_IMM_8

                    if(ins->child->op->format & O_MONO_AND_LIT) {
                        link<token>* r1 = t->next;
                        if(r1->child->type == TK_LABEL_USAGE) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_REF)) {
                                printf("got label argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered label usage '%s' on line %lli of %s but opcode does not expect reference literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_QWORD) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                printf("got 64-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered QWORD literal %s on line %lli of %s but opcode cannot accept 64-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_DWORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 32-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("got 32-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered DWORD literal %s on line %lli of %s but opcode cannot accept 32-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_WORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 16-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 16-bit literal argument %s=%llu to 32-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("got 16-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered WORD literal %s on line %lli of %s but opcode cannot accept 16-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_BYTE) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 8-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 8-bit literal argument %s=%llu to 32-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("upsizing 8-bit literal argument %s=%llu to 16-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            }
                        } else if(r1->child->type == TK_NAME) {
                            link<label>* s = labels;
                            link<label>* mylabel = NULL;
                            while(s != NULL) {
                                if(strcmp(s->child->text, r1->child->text) == 0) {
                                    fprintf(stderr, " -- label '%s' with address %016llx on line %lli of %s used on line %lli of %s\n", s->child->text, s->child->address, s->child->token->line, s->child->token->filename, ln, filename);
                                    mylabel = s;
                                    break;
                                }
                                s = s->next;
                            }

                            if(mylabel == NULL) {
                                fprintf(stderr, " -- undefined name '%s' on line %lli of %s\n", r1->child->text, ln, filename);
                                return 1;
                            } else {
                                ins->child->literal = r1->child;
                                r1->child->label = mylabel->child;
                                r1->child->literal_numeric_value = r1->child->label->address;
                                if(mylabel->child->address == (unsigned long long)-1) {
                                    printf(" -- unpatched address; adding to patch queue\n");
                                    link<patch_address>* new_patch = new link<patch_address>;
                                    new_patch->child = new patch_address;
                                    new_patch->child->instruction = ins->child;
                                    new_patch->child->label = mylabel->child;
                                    new_patch->next = unpatched;
                                    unpatched = new_patch;
                                }
                                TRAILER_SIZE = 4;
                            }
                        }
                    } // O_MONO_AND_LIT

                    if(ins->child->op->format & O_DUO_AND_LIT) {
                        link<token>* r2 = t->next;
                        if(r2->child->type == TK_LABEL_USAGE) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_REF)) {
                                printf("got label argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered label usage '%s' on line %lli of %s but opcode does not expect reference literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_QWORD) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                printf("got 64-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered QWORD literal %s on line %lli of %s but opcode cannot accept 64-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_DWORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 32-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("got 32-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered DWORD literal %s on line %lli of %s but opcode cannot accept 32-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_WORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 16-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 16-bit literal argument %s=%llu to 32-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("got 16-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered WORD literal %s on line %lli of %s but opcode cannot accept 16-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_BYTE) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 8-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 8-bit literal argument %s=%llu to 32-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("upsizing 8-bit literal argument %s=%llu to 16-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                ins->child->literal = r2->child;

                                t = t->next;
                            }
                        } else if(r2->child->type == TK_NAME) {
                            link<label>* s = labels;
                            link<label>* mylabel = NULL;
                            while(s != NULL) {
                                if(strcmp(s->child->text, r2->child->text) == 0) {
                                    fprintf(stderr, " -- label '%s' with address %016llx on line %lli of %s used on line %lli of %s\n", s->child->text, s->child->address, s->child->token->line, s->child->token->filename, ln, filename);
                                    mylabel = s;
                                    break;
                                }
                                s = s->next;
                            }

                            if(mylabel == NULL) {
                                fprintf(stderr, " -- undefined name '%s' on line %lli of %s\n", r2->child->text, ln, filename);
                                return 1;
                            } else {
                                ins->child->literal = r2->child;
                                r2->child->label = mylabel->child;
                                r2->child->literal_numeric_value = r2->child->label->address;
                                if(mylabel->child->address == (unsigned long long)-1) {
                                    printf(" -- unpatched address; adding to patch queue\n");
                                    link<patch_address>* new_patch = new link<patch_address>;
                                    new_patch->child = new patch_address;
                                    new_patch->child->instruction = ins->child;
                                    new_patch->child->label = mylabel->child;
                                    new_patch->next = unpatched;
                                    unpatched = new_patch;
                                }
                                TRAILER_SIZE = 4;
                            }
                        }
                    } // O_DUO_AND_LIT

                    if(ins->child->op->format & O_UNFINISHED) {
                        fprintf(stderr, "Unimplemented: unfinished instruction %s on line %lli of %s\n", ins->child->text, ln, filename);
                        return 1;
                    } // O_UNFINISHED

                    if(ins->child->op->format & O_LIT_ONLY) {
                        fprintf(stderr, "Unimplemented: lit-only instruction %s on line %lli of %s\n", ins->child->text, ln, filename);
                        return 1;
                    } // O_LIT_ONLY

                    if(ins->child->op->format & (O_ARITHMETIC_DUO | O_ARITHMETIC_TRIO)) {
                        link<token>* r0 = t->next;
                        link<token>* r1 = r0->next;

                        if(r0->child->line != ln || r0->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r0->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                            return 1;
                        } else if(r0->child->reg->number > 7) {
                            fprintf(stderr, " -- arithmetic operation '%s' on line %lli of %s can only use data registers (a-h); you provided a system register (%s)\n", ins->child->text, ln, filename, r0->child->reg->name);
                            return 1;
                        } else {
                            printf("got first register argument %s (r%02x)\n", r0->child->text, r0->child->reg->number);
                            opcode_emission = (opcode_emission & 0xfff8) | ((r0->child->reg->number & 7) << 0);
                            ins->child->arg_0 = r0->child;
                            t = t->next;
                        }

                        if(r1->child->line != ln || r1->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r1->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s (did you mean the .i version?)\n", r1->child->text, ln, filename);
                            return 1;
                        } else if(r1->child->reg->number > 7) {
                            fprintf(stderr, " -- arithmetic operation '%s' on line %lli of %s can only use data registers (a-h); you provided a system register (%s)\n", ins->child->text, ln, filename, r1->child->reg->name);
                            return 1;
                        } else {
                            printf("got second register argument %s (r%02x)\n", r1->child->text, r1->child->reg->number);
                            opcode_emission = (opcode_emission & 0xffc7) | ((r1->child->reg->number & 7) << 3);
                            ins->child->arg_1 = r1->child;
                            t = t->next;
                        }
                    } // O_ARITHMETIC_DUO | TRIO

                    if(ins->child->op->format & O_ARITHMETIC_TRIO) {
                        link<token>* r2 = t->next;
                        if(r2->child->line != ln || r2->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r2->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r2->child->text, ln, filename);
                            return 1;
                        } else if(r2->child->reg->number > 7) {
                            fprintf(stderr, " -- arithmetic operation '%s' on line %lli of %s can only use data registers (a-h); you provided a system register (%s)\n", ins->child->text, ln, filename, r2->child->reg->name);
                            return 1;
                        } else {
                            printf("got third register argument %s (r%02x)\n", r2->child->text, r2->child->reg->number);
                            opcode_emission = (opcode_emission & 0xfe3f) | ((r2->child->reg->number & 7) << 6);
                            ins->child->arg_2 = r2->child;
                            t = t->next;
                        }
                    } // O_ARITHMETIC_TRIO

                    if(ins->child->op->format & O_ARITHMETIC_LIT) {
                        link<token>* r0 = t->next;

                        if(r0->child->line != ln || r0->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r0->child->type != TK_REGNAME) {
                            fprintf(stderr, " -- expected register name but got '%s' on line %lli of %s\n", r0->child->text, ln, filename);
                            return 1;
                        } else if(r0->child->reg->number > 7) {
                            fprintf(stderr, " -- arithmetic operation '%s' on line %lli of %s can only use data registers (a-h); you provided a system register (%s)\n", ins->child->text, ln, filename, r0->child->reg->name);
                            return 1;
                        } else {
                            printf("got first register argument %s (r%02x)\n", r0->child->text, r0->child->reg->number);
                            opcode_emission = (opcode_emission & 0xfff8) | ((r0->child->reg->number & 7) << 0);
                            ins->child->arg_0 = r0->child;
                            t = t->next;
                        }

                        link<token>* r1 = t->next;
                        if(r1->child->type == TK_LABEL_USAGE) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_REF)) {
                                printf("got label argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered label usage '%s' on line %lli of %s but opcode does not expect reference literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_QWORD) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                printf("got 64-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered QWORD literal %s on line %lli of %s but opcode cannot accept 64-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_DWORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 32-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("got 32-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered DWORD literal %s on line %lli of %s but opcode cannot accept 32-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_WORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 16-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 16-bit literal argument %s=%llu to 32-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("got 16-bit literal argument %s=%llu\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered WORD literal %s on line %lli of %s but opcode cannot accept 16-bit literal\n", r1->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r1->child->type == TK_LIT_BYTE) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 8-bit literal argument %s=%llu to 64-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 8-bit literal argument %s=%llu to 32-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("upsizing 8-bit literal argument %s=%llu to 16-bit\n", r1->child->text, r1->child->literal_numeric_value);
                                ins->child->literal = r1->child;

                                t = t->next;
                            }
                        } else if(r1->child->type == TK_NAME) {
                            link<label>* s = labels;
                            link<label>* mylabel = NULL;
                            while(s != NULL) {
                                if(strcmp(s->child->text, r1->child->text) == 0) {
                                    fprintf(stderr, " -- label '%s' with address %016llx on line %lli of %s used on line %lli of %s\n", s->child->text, s->child->address, s->child->token->line, s->child->token->filename, ln, filename);
                                    mylabel = s;
                                    break;
                                }
                                s = s->next;
                            }

                            if(mylabel == NULL) {
                                fprintf(stderr, " -- undefined name '%s' on line %lli of %s\n", r1->child->text, ln, filename);
                                return 1;
                            } else {
                                ins->child->literal = r1->child;
                                r1->child->label = mylabel->child;
                                r1->child->literal_numeric_value = r1->child->label->address;
                                if(mylabel->child->address == (unsigned long long)-1) {
                                    printf(" -- unpatched address; adding to patch queue\n");
                                    link<patch_address>* new_patch = new link<patch_address>;
                                    new_patch->child = new patch_address;
                                    new_patch->child->instruction = ins->child;
                                    new_patch->child->label = mylabel->child;
                                    new_patch->next = unpatched;
                                    unpatched = new_patch;
                                }
                                TRAILER_SIZE = 4;
                            }
                        }
                    } // O_ARITHMETIC_LIT
                    
                    if(ins->child->op->format & O_TRIO_OR_LIT) {
                        link<token>* r2 = t->next;
                        if(r2->child->line != ln || r2->child->filename != filename) {
                            fprintf(stderr, " -- expected register name but got EOL on line %lli of %s\n", ln, filename);
                            return 1;
                        } else if(r2->child->type == TK_REGNAME) {
                            printf("got third register argument %s (r%02x)\n", r2->child->text, r2->child->reg->number);
                            opcode_emission = (opcode_emission & 0xf0ff) | (((unsigned short)(r2->child->reg->number) & 0xf) << 8);
                            ins->child->arg_2 = r2->child;
                            t = t->next;
                        } else if(r2->child->type == TK_LABEL_USAGE) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_REF)) {
                                printf("got label argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered label usage '%s' on line %lli of %s but opcode does not expect reference literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_QWORD) {
                            TRAILER_SIZE = 4;
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                printf("got 64-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered QWORD literal %s on line %lli of %s but opcode cannot accept 64-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_DWORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 32-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("got 32-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered DWORD literal %s on line %lli of %s but opcode cannot accept 32-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_WORD) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 16-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 16-bit literal argument %s=%llu to 32-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("got 16-bit literal argument %s=%llu\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered WORD literal %s on line %lli of %s but opcode cannot accept 16-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_LIT_BYTE) {
                            if(ins->child->op->format & (O_LIT_64 | O_LIT_REF)) {
                                TRAILER_SIZE = 4;
                                printf("upsizing 8-bit literal argument %s=%llu to 64-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_32)) {
                                TRAILER_SIZE = 2;
                                printf("upsizing 8-bit literal argument %s=%llu to 32-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else if(ins->child->op->format & (O_LIT_16)) {
                                TRAILER_SIZE = 1;
                                printf("upsizing 8-bit literal argument %s=%llu to 16-bit\n", r2->child->text, r2->child->literal_numeric_value);
                                opcode_emission = opcode_emission | 0x0f00;
                                ins->child->literal = r2->child;

                                t = t->next;
                            } else {
                                fprintf(stderr, " -- encountered byte literal %s on line %lli of %s but opcode cannot accept 8-bit literal\n", r2->child->text, ln, filename);
                                return 1;
                            }
                        } else if(r2->child->type == TK_NAME) {
                            link<label>* s = labels;
                            link<label>* mylabel = NULL;
                            while(s != NULL) {
                                if(strcmp(s->child->text, r2->child->text) == 0) {
                                    fprintf(stderr, " -- label '%s' with address %016llx on line %lli of %s used on line %lli of %s\n", s->child->text, s->child->address, s->child->token->line, s->child->token->filename, ln, filename);
                                    mylabel = s;
                                    break;
                                }
                                s = s->next;
                            }

                            if(mylabel == NULL) {
                                fprintf(stderr, " -- undefined name '%s' on line %lli of %s\n", r2->child->text, ln, filename);
                                return 1;
                            } else {
                                ins->child->literal = r2->child;
                                r2->child->label = mylabel->child;
                                r2->child->literal_numeric_value = r2->child->label->address;
                                if(mylabel->child->address == (unsigned long long)-1) {
                                    printf(" -- unpatched address; adding to patch queue\n");
                                    link<patch_address>* new_patch = new link<patch_address>;
                                    new_patch->child = new patch_address;
                                    new_patch->child->instruction = ins->child;
                                    new_patch->child->label = mylabel->child;
                                    new_patch->next = unpatched;
                                    unpatched = new_patch;
                                }
                                TRAILER_SIZE = 4;
                                opcode_emission = opcode_emission | 0x0f00;
                            }
                        } else {
                            fprintf(stderr, " -- expected register name, label usage, or numeric literal but got '%s' on line %lli of %s\n", r2->child->text, ln, filename);
                            return 1;
                        }
                    } // O_TRIO_OR_LIT

                    if(TRAILER_SIZE) {
                        printf("encoding with %llu-word trailer\n", TRAILER_SIZE);
                    } else {
                        printf("encoding without trailer\n");
                    }
                    ins->child->output_size = (1 + TRAILER_SIZE) * 2;
                    ins->child->output = (char*)malloc(sizeof(char) * ins->child->output_size);
                    ins->child->output[0] = ((opcode_emission) >> 8) & 0xff;
                    ins->child->output[1] = (opcode_emission) & 0xff;
                    if(TRAILER_SIZE == 4) {
                        ins->child->output[2] = ((ins->child->literal->literal_numeric_value) >> 56) & 0xff;
                        ins->child->output[3] = ((ins->child->literal->literal_numeric_value) >> 48) & 0xff;
                        ins->child->output[4] = ((ins->child->literal->literal_numeric_value) >> 40) & 0xff;
                        ins->child->output[5] = ((ins->child->literal->literal_numeric_value) >> 32) & 0xff;
                        ins->child->output[6] = ((ins->child->literal->literal_numeric_value) >> 24) & 0xff;
                        ins->child->output[7] = ((ins->child->literal->literal_numeric_value) >> 16) & 0xff;
                        ins->child->output[8] = ((ins->child->literal->literal_numeric_value) >>  8) & 0xff;
                        ins->child->output[9] = ((ins->child->literal->literal_numeric_value) >>  0) & 0xff;
                    } else if(TRAILER_SIZE == 2) {
                        ins->child->output[2] = ((ins->child->literal->literal_numeric_value) >> 24) & 0xff;
                        ins->child->output[3] = ((ins->child->literal->literal_numeric_value) >> 16) & 0xff;
                        ins->child->output[4] = ((ins->child->literal->literal_numeric_value) >>  8) & 0xff;
                        ins->child->output[5] = ((ins->child->literal->literal_numeric_value) >>  0) & 0xff;
                    } else if(TRAILER_SIZE == 1) {
                        ins->child->output[2] = ((ins->child->literal->literal_numeric_value) >>  8) & 0xff;
                        ins->child->output[3] = ((ins->child->literal->literal_numeric_value) >>  0) & 0xff;
                    }
                    // ins->child->output[2 + TRAILER_SIZE] = 0;
                    printf("line %04llu: \t%s -> %04x\n", ln, o->text, opcode_emission);
                    address += 1 + TRAILER_SIZE;

                    link<emission>* new_emission = new link<emission>;
                    new_emission->child = new emission;
                    new_emission->child->origin = ins->child;
                    new_emission->child->text = ins->child->output;
                    new_emission->child->size = ins->child->output_size;
                    if(last_emission != NULL)
                        last_emission->next = new_emission;    
                    else
                        emissions = new_emission;
                    
                    new_emission->next = NULL;
                    last_emission = new_emission;
                } else if(strcmp(t->child->text, "repeat") == 0) {
                    fpos_t count = t->next->child->literal_numeric_value;
                    if(count == 0 || (t->next->child->line != ln || t->next->child->filename != filename)) {
                        fprintf(stderr, "Warning: missing, invalid, or zero argument to 'repeat' on line %lli of %s\n", ln, filename);
                    } else {
                        last_emission->child->repeats = count * last_emission->child->size;
                        printf("Adding %lli byte(s) for 'repeat' on line %lli\n", last_emission->child->repeats, ln);
                        address += (last_emission->child->repeats) / 2;
                    }
                } else if(strcmp(t->child->text, "pad") == 0) {
                    fpos_t count = t->next->child->literal_numeric_value;
                    if(count == 0 || (t->next->child->line != ln || t->next->child->filename != filename)) {
                        fprintf(stderr, "Warning: missing, invalid, or zero argument to 'pad' on line %lli of %s\n", ln, filename);
                    } else {
                        last_emission->child->repeats = (count - address) * 2;
                        printf("Adding %lli byte(s) for 'pad' on line %lli of %s\n", last_emission->child->repeats, ln, filename);
                        address += (last_emission->child->repeats) / 2;
                    }
                } else {
                    fprintf(stderr, " -- unknown instruction %s on line %lli of %s\n", t->child->text, ln, filename);
                    return 1;
                }
            } else if(t->child->type == TK_LIT_STRING
                   || t->child->type == TK_LIT_QWORD
                   || t->child->type == TK_LIT_DWORD
                   || t->child->type == TK_LIT_WORD
                   || t->child->type == TK_LIT_BYTE) {
                
                link<emission>* new_emission = new link<emission>;
                new_emission->child = new emission;
                new_emission->child->origin = t->child;
                // new_emission->child->text = ins->child->output;
                // new_emission->child->size = ins->child->output_size;
                if(last_emission != NULL)
                    last_emission->next = new_emission;    
                else
                    emissions = new_emission;
                
                new_emission->next = NULL;
                last_emission = new_emission;

                if(t->child->type == TK_LIT_STRING) {
                    t->child->address = address;
                    address += (t->child->output_size) / 2;
                    
                    printf("%4llx\tSTR\t%04llx bytes / original text \"%s\"\n", ln, t->child->output_size, t->child->text);

                    new_emission->child->text = t->child->output;
                    new_emission->child->size = t->child->output_size;
                } else if(t->child->type == TK_LIT_QWORD) {
                    new_emission->child->size = 8;
                    new_emission->child->text = (char*)malloc(sizeof(char) * 8);
                    new_emission->child->text[0] = ((t->child->literal_numeric_value) >> 56) & 0xff;
                    new_emission->child->text[1] = ((t->child->literal_numeric_value) >> 48) & 0xff;
                    new_emission->child->text[2] = ((t->child->literal_numeric_value) >> 40) & 0xff;
                    new_emission->child->text[3] = ((t->child->literal_numeric_value) >> 32) & 0xff;
                    new_emission->child->text[4] = ((t->child->literal_numeric_value) >> 24) & 0xff;
                    new_emission->child->text[5] = ((t->child->literal_numeric_value) >> 16) & 0xff;
                    new_emission->child->text[6] = ((t->child->literal_numeric_value) >>  8) & 0xff;
                    new_emission->child->text[7] = ((t->child->literal_numeric_value) >>  0) & 0xff;
                    printf(" -- encoding 64-bit numeric literal '%s' as quadruple word\n", t->child->text);
                    address += 4;
                } else if(t->child->type == TK_LIT_DWORD) {
                    new_emission->child->size = 4;
                    new_emission->child->text = (char*)malloc(sizeof(char) * 4);
                    new_emission->child->text[0] = ((t->child->literal_numeric_value) >> 24) & 0xff;
                    new_emission->child->text[1] = ((t->child->literal_numeric_value) >> 16) & 0xff;
                    new_emission->child->text[2] = ((t->child->literal_numeric_value) >>  8) & 0xff;
                    new_emission->child->text[3] = ((t->child->literal_numeric_value) >>  0) & 0xff;
                    address += 2;
                    printf(" -- encoding 32-bit numeric literal '%s' as double word\n", t->child->text);
                } else if(t->child->type == TK_LIT_WORD) {
                    new_emission->child->size = 2;
                    new_emission->child->text = (char*)malloc(sizeof(char) * 2);
                    new_emission->child->text[0] = ((t->child->literal_numeric_value) >>  8) & 0xff;
                    new_emission->child->text[1] = ((t->child->literal_numeric_value) >>  0) & 0xff;
                    address += 1;
                    printf(" -- encoding 16-bit numeric literal '%s' as word\n", t->child->text);
                } else if(t->child->type == TK_LIT_BYTE) {
                    new_emission->child->size = 2;
                    new_emission->child->text = (char*)malloc(sizeof(char) * 2);
                    new_emission->child->text[0] = 0;
                    new_emission->child->text[1] = t->child->literal_numeric_value;
                    address += 1;
                    printf(" -- encoding 8-bit numeric literal '%s' as word\n", t->child->text);
                }
            } else {
                printf("[unimplemented?] not sure what to do with '%s' on line %lli of %s\n", t->child->text, ln, filename);
                return 1;
            }
        }
        t = t->next;
    }
    return 0;
}