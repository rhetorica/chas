#ifndef OPCODE_H
#define OPCODE_H

struct opcode {
	unsigned short number;
	const char* text;
    unsigned short emission;
    unsigned long format;
};

// opcode is nullary:
#define O_ZERO          0x00000000
// opcode takes an r register argument, in the last 4 bits:
#define O_MONO          0x00000001
// opcode takes 1 64-bit literal argument:
#define O_LIT_64        0x00000002
// opcode takes 1 32-bit literal argument:
#define O_LIT_32        0x00000004
// opcode takes 1 16-bit literal argument:
#define O_LIT_16        0x00000008
// opcode takes an s register argument:
#define O_DUO           0x00000010
// opcode takes a T register argument or a QWORD literal if T is 1111:
#define O_TRIO          0x00000020
// opcode takes a literal bit number in the middle ('clear' and 'set'):
#define O_FLAG          0x00000040
// opcode takes an R register argument, in the last 4 bits (or 64-bit literal):
#define O_MONO_OR_LIT   0x00000080
// opcode takes an 8-bit literal inside the last 8 bits (int and cout):
#define O_OPT_IMM_8     0x00000100
// opcode takes an r register argument and a literal
#define O_MONO_AND_LIT  0x00000200
// opcode takes only an s register argument:
#define O_DUO_ONLY      0x00000800
// opcode is from the new arithmetic family and takes two args (SSSRRR):
#define O_ARITHMETIC_DUO 0x00001000
// opcode is from the new arithmetic family and has trailer (RRR):
#define O_ARITHMETIC_LIT 0x00002000
// opcode may not take STATUS as first operand:
#define O_STATUS_FORBIDDEN 0x00004000
// opcode lit is an address:
#define O_LIT_REF       0x00008000
// opcode needs to be implemented properly:
#define O_UNFINISHED    0xff000000
// opcode only takes literal:
#define O_LIT_ONLY      0x00010000


void init_opcodes();
extern link<opcode>* opcodes;
opcode* find_opcode(char* text);

#endif //OPCODE_H
