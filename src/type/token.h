
#ifndef TOKEN_H
#define TOKEN_H

#define TK_OPCODE 1
#define TK_COMMA 2
#define TK_REGNAME 3
#define TK_LIT_BIT_INDEX 4
#define TK_LABEL 5
#define TK_BEGIN 6
#define TK_NAME 7
#define TK_LIT_STRING 8
#define TK_LABEL_USAGE 9
#define TK_PLUS 10
#define TK_MINUS 11
#define TK_LIT_QWORD 12
#define TK_LIT_DWORD 13
#define TK_LIT_WORD 14
#define TK_LIT_BYTE 15
#define TK_DEFINITION 16

struct token {
	unsigned short type = 0;
	char* text;
	fpos_t offset;
	fpos_t line;
	fpos_t number;
    fpos_t address;
    char* output = NULL;
    fpos_t output_size;
    opcode* op;
    token* arg_0;
    token* arg_1;
    token* arg_2;
    token* literal;
    token* immediate;
    label* label;
    reg* reg;
    unsigned long long literal_numeric_value;
    char* filename;
};

char* make_literal_output(unsigned long long source, unsigned short bytes);
link<token>* tokenize(char* raw_text, fpos_t size);

#endif // TOKEN_H
