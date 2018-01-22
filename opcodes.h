
#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum INSTRUCTION {
	// Basic
	I_MOV = 0,
	I_MOV64,
	I_MOV8,
	I_MOV16,
	I_PUSH,
	I_POP,
	I_PUSH64,
	I_POP64,
	I_NOP,
	// Integer Arithmetic
	I_ADD,
	I_SUB,
	I_MUL,
	I_DIV,
	I_MOD,
	I_SHL,
	I_LSHR,
	I_ASHR,
	I_MULU,
	I_DIVU,
	// Logic
	I_XOR,
	I_OR,
	I_AND,
	I_NOT,
	I_NOR,
	I_NAND,
	// Compare
	I_CMP,
	I_CMPS,
	I_CMP64,
	I_CMP64S,
	I_CMPF,
	I_CMP64F,
	// Flow
	I_JMP,
	I_JGE,
	I_JLE,
	I_JE,
	I_JNE,
	I_JG,
	I_JL,
	I_JZ,
	I_JNZ,
	I_CALL,
	I_CGE,
	I_CLE,
	I_CE,
	I_CNE,
	I_CG,
	I_CL,
	I_CZ,
	I_CNZ,
	I_RETN,
	// Special
	I_INT,
	I_SETIH,
	I_HALT,
	I_RESET,
	I_TRAP,
	I_ENCOFF,
	I_ENCON,
	I_PUSHREG,
	I_POPREG,
	I_ENCRETN,
	// Integer Arithmetic 64bit
	I_ADD64,
	I_SUB64,
	I_MUL64,
	I_DIV64,
	I_MOD64,
	I_SHL64,
	I_LSHR64,
	I_ASHR64,
	I_MULU64,
	I_DIVU64,
	// Float Arithmetic 32bit
	I_ADDF,
	I_SUBF,
	I_MULF,
	I_DIVF,
	// Float Arithmetic 64bit
	I_ADD64F,
	I_SUB64F,
	I_MUL64F,
	I_DIV64F,
	// Extra
	I_DEBUG,
	I_SOFF1,
	I_SOFF2,
	I_SOFF12,
	I_ROFF1,
	I_ROFF2,
	I_ROFF12,
};

#define II_MAX 84

enum ITYPE {
	IT_RRVV = 0,
	IT_N,
	IT_RRVV64,
	IT_V8,
	IT_V8V8,
	IT_INVALID
};

static uint8_t ITYPES[] = {
	// Basic
	IT_RRVV,
	IT_RRVV64,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV64,
	IT_RRVV64,
	IT_N,
	// Integer Arithmetic
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	// Logic
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	// Compare
	IT_RRVV,
	IT_RRVV,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV,
	IT_RRVV64,
	// Flow
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_N,
	// Special
	IT_RRVV,
	IT_RRVV,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	// Integer Arithmetic 64bit
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	// Float Arithmetic 32bit
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	// Float Arithmetic 64bit
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	IT_RRVV,
	// Extra
	IT_N,
	IT_V8,
	IT_V8,
	IT_V8V8,
	IT_N,
	IT_N,
	IT_N,
};

#endif // OPCODES_H_INCLUDED
