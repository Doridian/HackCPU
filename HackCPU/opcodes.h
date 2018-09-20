
#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum INSTRUCTION {
	I_MOV = 0,
	I_MOV8,
	I_MOV16,
	I_PUSH,
	I_POP,
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
	I_XOR,
	I_OR,
	I_AND,
	I_NOT,
	I_NOR,
	I_NAND,
	I_CMP,
	I_CMPS,
	I_CMPF,
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
	I_RETNA,
	I_INT,
	I_SETIH,
	I_ADDF,
	I_SUBF,
	I_MULF,
	I_DIVF,
	I_ADD64F,
	I_SUB64F,
	I_MUL64F,
	I_DIV64F,
	I_CPUID,
	I_RAMSIZE,
	I_MOV64 = 64,
	I_PUSH64,
	I_POP64,
	I_CMP64,
	I_CMP64S,
	I_CMP64F,
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
	I_POPNIL = 128,
	I_POPNIL64,
	I_NOP,
	I_RETN,
	I_HALT,
	I_RESET,
	I_TRAP,
	I_ENCOFF,
	I_ENCON,
	I_PUSHREG,
	I_POPREG,
	I_DEBUG,
	I_RETNAC = 192,
	I_FIRST_INVALID
};

enum ITYPE {
	IT_RRVV = 0,
	IT_N,
	IT_RRVV64,
	IT_I8,
	IT_I8I8,
	IT_U8,
	IT_U8U8,
	IT_INVALID
};

static uint8_t ITYPES[] = {
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
	IT_RRVV,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
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
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_RRVV64,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_N,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_INVALID,
	IT_U8,
};

#endif // OPCODES_H_INCLUDED
