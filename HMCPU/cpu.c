#include <stdint.h>
#include <stddef.h>
#include "cpu.h"
#include "registers.h"
#include "memory.h"
#include "opcodes.h"
#include "bootloader.h"
#include "io.h"

uint8_t cpu_needs_reset = 1;

static void memclear(void *ptr, size_t num) {
	uint8_t *ptrb = (uint8_t *)ptr;
	while (num--) {
		*ptrb++ = 0;
	}
}

typedef struct regregvalval64_t {
	union {
		uint64_t* reg1;
		int64_t* reg1s;
		double* reg1f;
	};
	union {
		uint64_t* reg2;
		int64_t* reg2s;
		double* reg2f;
	};
	union {
		uint64_t reg1val;
		int64_t reg1vals;
		double reg1valf;
	};
	union {
		uint64_t reg2val;
		int64_t reg2vals;
		double reg2valf;
	};
} regregvalval64_t;

typedef struct regregvalval32_t {
	union {
		uint32_t* reg1;
		int32_t* reg1s;
		float* reg1f;
	};
	union {
		uint32_t* reg2;
		int32_t* reg2s;
		float* reg2f;
	};
	union {
		uint32_t reg1val;
		int32_t reg1vals;
		float reg1valf;
	};
	union {
		uint32_t reg2val;
		int32_t reg2vals;
		float reg2valf;
	};
} regregvalval32_t;

void cpu_reset() {
	memclear(&r, sizeof(r));
	int i;
	for (i = 0; i < sizeof(BOOTLOADER); i++) {
		m[i] = BOOTLOADER[i];
	}
	cpu_needs_reset = 0;
}

static void devzero_write(struct iostream_t* io, uint8_t i) { }
static uint8_t devzero_read(struct iostream_t* io) { return 0; }

uint8_t dummyrom[1] = { 0x00 };

static uint8_t dummyrom_read(struct iostream_t* io) {
	return dummyrom[io->rptr];
}

void cpu_init() {
	//memclear(m, RAM_SIZE);

	// stdout
	io[IO_STDOUT].rptr = 0;
	io[IO_STDOUT].wptr = 0;
	io[IO_STDOUT].length = 0;
	io[IO_STDOUT].read = 0;
	io[IO_STDOUT].write = devzero_write;
	io[IO_STDOUT].flags = 0;

	// ROM
	io[IO_ROM].rptr = 0;
	io[IO_ROM].wptr = 0;
	io[IO_ROM].length = sizeof(dummyrom);
	io[IO_ROM].read = dummyrom_read;
	io[IO_ROM].write = 0;
	io[IO_ROM].flags = IO_FLAG_RPTR_GET|IO_FLAG_RPTR_SET|IO_FLAG_LENGTH|IO_FLAG_RESET;

	// stdin
	io[IO_STDIN].rptr = 0;
	io[IO_STDIN].wptr = 0;
	io[IO_STDIN].length = 0;
	io[IO_STDIN].read = devzero_read;
	io[IO_STDIN].write = 0;
	io[IO_STDIN].flags = 0;

	// /dev/zero
	io[3].rptr = 0;
	io[3].wptr = 0;
	io[3].length = 0;
	io[3].read = devzero_read;
	io[3].write = devzero_write;
	io[3].flags = IO_FLAG_WPTR_GET|IO_FLAG_WPTR_SET|IO_FLAG_RPTR_GET|IO_FLAG_RPTR_SET|IO_FLAG_LENGTH|IO_FLAG_RESET;

	cpu_reset();
}

static uint8_t iread8() {
	uint32_t opc = r.pc++;
	if (opc >= RAM_SIZE) {
		r.pc = 1;
		opc = 0;
	}
	uint8_t raw = m[opc];
	if (r.flagr & FLAG_ENCON) {
		return raw ^ (uint8_t)((r.encreg12 >> ((opc % 8) << 3)) & 0xFF);
	}
	return raw;
}

static int8_t siread8() {
	uint8_t u8 = iread8();
	return *(int8_t*)&u8;
}

#define __iread8_64() ((uint64_t)iread8())
#define __iread8_32() ((uint32_t)iread8())

static uint32_t iread32() {
	return __iread8_32() + (__iread8_32() << 8) + (__iread8_32() << 16) + (__iread8_32() << 24);
}

static uint64_t iread64() {
	return __iread8_64() + (__iread8_64() << 8) + (__iread8_64() << 16) + (__iread8_64() << 24) + (__iread8_64() << 32) + (__iread8_64() << 40) + (__iread8_64() << 48) + (__iread8_64() << 56);
}

static regregvalval32_t ireadrrvv32() {
	uint8_t regs = iread8();
	uint8_t r1 = (regs >> 4) & 0x0F;
	uint8_t r2 = regs & 0x0F;
	regregvalval32_t res;
	res.reg1 = r.u + r1; // TODO: Make sure this adds 2 * res.reg1, not 1!
	res.reg2 = r.u + r2; // TODO: Make sure this adds 2 * res.reg2, not 1!
	if (r1 == CREG_ID) {
		res.reg1 = (uint32_t*)(m + (r.pc % RAM_SIZE));
		res.reg1val = iread32();
	} else if (r1 == MREG_ID) {
		res.reg1 = (uint32_t*)(m + ((r.u[iread8() & 0x0F] + r.r1offset) % RAM_SIZE));
		if (r.flagr | FLAG_RSTOFF1) {
			r.flagr &= ~FLAG_RSTOFF1;
			r.r1offset = 0;
		}
		res.reg1val = *res.reg1;
	} else if (r1 == MREGC_ID) {
		res.reg1 = (uint32_t*)(m + (iread32() % RAM_SIZE));
		res.reg1val = *(uint32_t*)res.reg1;
	} else if (r1 >= REGISTERS_SIZE) {
		res.reg1 = NULL;
		res.reg1val = 0;
	} else {
		res.reg1val = *res.reg1;
	}
	if (r2 == CREG_ID) {
		res.reg2 = (uint32_t*)(m + (r.pc % RAM_SIZE));
		res.reg2val = iread32();
	} else if (r2 == MREG_ID) {
		res.reg2 = (uint32_t*)(m + ((r.u[iread8() & 0x0F] + r.r2offset) % RAM_SIZE));
		if (r.flagr | FLAG_RSTOFF2) {
			r.flagr &= ~FLAG_RSTOFF2;
			r.r2offset = 0;
		}
		res.reg2val = *res.reg2;
	} else if (r2 == MREGC_ID) {
		res.reg2 = (uint32_t*)(m + (iread32() % RAM_SIZE));
		res.reg2val = *(uint32_t*)res.reg2;
	} else if (r2 >= REGISTERS_SIZE) {
		res.reg2 = NULL;
		res.reg2val = 0;
	} else {
		res.reg2val = *res.reg2;
	}
	return res;
}

static regregvalval64_t ireadrrvv64() {
	uint8_t regs = iread8();
	uint8_t r1 = (regs >> 4) & 0x0F;
	uint8_t r2 = regs & 0x0F;
	regregvalval64_t res;
	res.reg1 = (uint64_t*)(r.u + r1); // TODO: Make sure this adds 2 * res.reg1, not 1!
	res.reg2 = (uint64_t*)(r.u + r2); // TODO: Make sure this adds 2 * res.reg2, not 1!
	if (r1 == CREG_ID) {
		res.reg1 = (uint64_t*)(m + (r.pc % RAM_SIZE));
		res.reg1val = iread64();
	} else if (r1 == MREG_ID) {
		res.reg1 = (uint64_t*)(m + ((r.u[iread8() & 0x0F] + r.r1offset) % RAM_SIZE));
		if (r.flagr | FLAG_RSTOFF1) {
			r.flagr &= ~FLAG_RSTOFF1;
			r.r1offset = 0;
		}
		res.reg1val = *(uint64_t*)res.reg1;
	} else if (r1 == MREGC_ID) {
		res.reg1 = (uint64_t*)(m + (iread32() % RAM_SIZE));
		res.reg1val = *(uint64_t*)res.reg1;
	} else if (r1 >= REGISTERS_SIZE) {
		res.reg1 = NULL;
		res.reg1val = 0;
	} else {
		res.reg1val = *(uint64_t*)res.reg1;
	}
	if (r2 == CREG_ID) {
		res.reg2 = (uint64_t*)(m + (r.pc % RAM_SIZE));
		res.reg2val = iread64();
	} else if (r2 == MREG_ID) {
		res.reg2 = (uint64_t*)(m + ((r.u[iread8() & 0x0F] + r.r2offset) % RAM_SIZE));
		if (r.flagr | FLAG_RSTOFF2) {
			r.flagr &= ~FLAG_RSTOFF2;
			r.r2offset = 0;
		}
		res.reg2val = *(uint64_t*)res.reg2;
	} else if (r2 == MREGC_ID) {
		res.reg2 = (uint64_t*)(m + (iread32() % RAM_SIZE));
		res.reg2val = *(uint64_t*)res.reg2;
	} else if (r2 >= REGISTERS_SIZE) {
		res.reg2 = NULL;
		res.reg2val = 0;
	} else {
		res.reg2val = *(uint64_t*)res.reg2;
	}
	return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

static void push(uint32_t i) {
	uint32_t* m32 = (uint32_t*)(m + (r.psp % RAM_SIZE));
	r.psp += 4;
	*m32 = i;
}

static void ipush(uint32_t i) {
	uint32_t* m32 = (uint32_t*)(m + (r.csp % RAM_SIZE));
	r.csp += 4;
	*m32 = i;
}

static void push64(uint64_t i) {
	uint64_t* m64 = (uint64_t*)(m + (r.psp % RAM_SIZE));
	r.psp += 8;
	*m64 = i;
}

static uint32_t pop() {
	r.psp -= 4;
	return *(uint32_t*)(m + (r.psp % RAM_SIZE));
}

static uint64_t pop64() {
	r.psp -= 8;
	return *(uint64_t*)(m + (r.psp % RAM_SIZE));
}

static uint32_t ipop() {
	r.csp -= 4;
	return *(uint32_t*)(m + (r.csp % RAM_SIZE));
}

#define DOCMP(a, b) r.flagr = (r.flagr & FLAG_NOCMP) | \
							  ((a < b) << 1) | \
							  (a == b);

#define IFEQ()  if   (r.flagr & FLAG_EQ)
#define IFNEQ() if (!(r.flagr & FLAG_EQ))

#define IFLT()  if   (r.flagr & FLAG_LT)
#define IFGTE() if (!(r.flagr & FLAG_LT))

#define IFLTE() if   (r.flagr & (FLAG_LT|FLAG_EQ))
#define IFGT()  if (!(r.flagr & (FLAG_LT|FLAG_EQ)))

#define DOJMP()	DOJMPP(1);
#define DOJMPZ()   DOJMPP(2);
#define DOJMPP(a)  { r.pc = rrvv32.reg ## a ## val; }
#define DOCALL()   DOCALLP(1);
#define DOCALLZ()  DOCALLP(2);
#define DOCALLP(a) { ipush(r.pc); DOJMPP(a); }

#define IFZ()  if (rrvv32.reg1val == 0)
#define IFNZ() if (rrvv32.reg1val != 0)

static uint8_t cpu_interrupt(uint8_t i) {
	if (i > 7) {
		return ERR_UNHANDLED_INTERRUPT;
	}
	uint32_t ioid = pop();
	if (ioid >= IOSTREAM_COUNT) {
		return ERR_INVALID_IO;
	}
	iostream_t iostr = io[ioid];
	uint32_t offset, len, x;

	switch (i) {
	case INT_IO_WRITE:
		offset = pop();
		len = pop();
		if (!iostr.write) {
			return ERR_INVALID_IO;
		}
		for (x = 0; x < len; x++) {
			if (x + offset >= RAM_SIZE) {
				return ERR_INVALID_IO;
			}
			iostr.write(&iostr, m[x + offset]);
			iostr.wptr++;
		}
		break;
	case INT_IO_READ:
		offset = pop();
		len = pop();
		if (!iostr.read) {
			return ERR_INVALID_IO;
		}
		for (x = 0; x < len; x++) {
			if (x + offset >= RAM_SIZE) {
				return ERR_INVALID_IO;
			}
			m[x + offset] = iostr.read(&iostr);
			iostr.rptr++;
		}
		break;
	case INT_IO_WPTR_SET:
		if (iostr.flags | IO_FLAG_WPTR_SET) {
			iostr.wptr = pop();
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RPTR_SET:
		if (iostr.flags | IO_FLAG_RPTR_SET) {
			iostr.rptr = pop();
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_WPTR_GET:
		if (iostr.flags | IO_FLAG_WPTR_GET) {
			push64(iostr.wptr);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RPTR_GET:
		if (iostr.flags | IO_FLAG_RPTR_GET) {
			push64(iostr.rptr);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_LENGTH_GET:
		if (iostr.flags | IO_FLAG_LENGTH) {
			push64(iostr.length);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RESET:
		if (iostr.flags | IO_FLAG_RESET) {
			iostr.wptr = 0;
			iostr.rptr = 0;
		} else {
			return ERR_INVALID_IO;
		}
		break;
	}
	return 0;
}

static uint8_t interrupt_nopush(uint8_t i) {
	if (r.ihbase == 0) {
		return cpu_interrupt(i);
	}
	uint32_t newpc = *(uint32_t*)(m + (((i << 1) + r.ihbase) % RAM_SIZE));
	if (newpc == 0) {
		return cpu_interrupt(i);
	}
	if (i & 0b10000000) {
		push(r.pc);
	}
	r.pc = newpc;
	return 0;
}

static uint8_t interrupt(uint8_t i) {
	uint8_t res = interrupt_nopush(i);
	if (res == ERR_UNHANDLED_INTERRUPT) {
		push(i);
	}
	return res;
}

uint8_t cpu_run() {
	while(1) {
		uint8_t res = cpu_step();
		if (res) {
			return res;
		}
	}
}

static uint8_t _cpu_step() {
	uint8_t op = iread8();

	if (op > II_MAX) {
		return interrupt(INT_ILLEGAL_OPCODE);
	}

	regregvalval32_t rrvv32;
	regregvalval64_t rrvv64;

	switch (ITYPES[op]) {
	case IT_RRVV:
		rrvv32 = ireadrrvv32();
		if (rrvv32.reg1 == NULL || rrvv32.reg2 == NULL) {
			return ERR_INVALID_REGISTER;
		}
		break;
	case IT_RRVV64:
		rrvv64 = ireadrrvv64();
		if (rrvv64.reg1 == NULL || rrvv64.reg2 == NULL) {
			return ERR_INVALID_REGISTER;
		}
		break;
	case IT_V8: // Handled by the insn itself
	case IT_V8V8: // Handled by the insn itself
	default:
	case IT_N:
		break;
	}

	switch (op) {
		// Basic
	case I_MOV:
		*rrvv32.reg1 = rrvv32.reg2val;
		break;
	case I_MOV64:
		*rrvv64.reg1 = rrvv64.reg2val;
		break;
	case I_PUSH:
		push(rrvv32.reg1val);
		break;
	case I_POP:
		*rrvv32.reg1 = pop();
		break;
	case I_PUSH64:
		push64(rrvv32.reg1val);
		break;
	case I_POP64:
		*rrvv64.reg1 = pop64();
		break;
	case I_NOP:
		// NOP
		break;
		// 32-bit Arithmetic
	case I_ADD:
		*rrvv32.reg1s += rrvv32.reg2vals;
		break;
	case I_SUB:
		*rrvv32.reg1s -= rrvv32.reg2vals;
		break;
	case I_MUL:
		*rrvv32.reg1s *= rrvv32.reg2vals;
		break;
	case I_DIV:
		*rrvv32.reg1s /= rrvv32.reg2vals;
		break;
	case I_MOD:
		*rrvv32.reg1s %= rrvv32.reg2vals;
		break;
	case I_SHL:
		*rrvv32.reg1s <<= rrvv32.reg2vals;
		break;
	case I_LSHR:
		*rrvv32.reg1 >>= rrvv32.reg2val;
		break;
	case I_ASHR:
		*rrvv32.reg1s >>= rrvv32.reg2vals;
		break;
	case I_MULU:
		*rrvv32.reg1 *= rrvv32.reg2val;
		break;
	case I_DIVU:
		*rrvv32.reg1 /= rrvv32.reg2val;
		break;
		// Logic
	case I_XOR:
		*rrvv32.reg1 ^= rrvv32.reg2val;
		break;
	case I_OR:
		*rrvv32.reg1 |= rrvv32.reg2val;
		break;
	case I_AND:
		*rrvv32.reg1 &= rrvv32.reg2val;
		break;
	case I_NOT:
		*rrvv32.reg1 = ~rrvv32.reg2val;
		break;
	case I_NOR:
		*rrvv32.reg1 |= ~rrvv32.reg2val;
		break;
	case I_NAND:
		*rrvv32.reg1 &= ~rrvv32.reg2val;
		break;
		// Compare
	case I_CMP:
		DOCMP(rrvv32.reg1val, rrvv32.reg2val);
		break;
	case I_CMPS:
		DOCMP(rrvv32.reg1vals, rrvv32.reg2vals);
		break;
	case I_CMP64:
		DOCMP(rrvv64.reg1val, rrvv64.reg2val);
		break;
	case I_CMP64S:
		DOCMP(rrvv64.reg1vals, rrvv64.reg2vals);
		break;
	case I_CMPF:
		DOCMP(rrvv32.reg1valf, rrvv32.reg2valf);
		break;
	case I_CMP64F:
		DOCMP(rrvv64.reg1valf, rrvv64.reg2valf);
		break;
		// Flow
	case I_JMP:
		DOJMP();
		break;
	case I_JGE:
		IFGTE() { DOJMP(); }
		break;
	case I_JLE:
		IFLTE() { DOJMP(); }
		break;
	case I_JE:
		IFEQ() { DOJMP(); }
		break;
	case I_JNE:
		IFNEQ() { DOJMP(); }
		break;
	case I_JG:
		IFGT() { DOJMP(); }
		break;
	case I_JL:
		IFLT() { DOJMP(); }
		break;
	case I_JZ:
		IFZ() { DOJMPZ(); }
		break;
	case I_JNZ:
		IFNZ() { DOJMPZ(); }
		break;
	case I_CALL:
		DOCALL();
		break;
	case I_CGE:
		IFGTE() { DOCALL(); }
		break;
	case I_CLE:
		IFLTE() { DOCALL(); }
		break;
	case I_CE:
		IFEQ() { DOCALL(); }
		break;
	case I_CNE:
		IFNEQ() { DOCALL(); }
		break;
	case I_CG:
		IFGT() { DOCALL(); }
		break;
	case I_CL:
		IFLT() { DOCALL(); }
		break;
	case I_CZ:
		IFZ() { DOCALLZ(); }
		break;
	case I_CNZ:
		IFNZ() { DOCALLZ(); }
		break;
	case I_RETN:
		r.pc = ipop();
		break;
		// Special
	case I_INT:
		return interrupt(rrvv32.reg1val);
	case I_SETIH:
		*(uint16_t*)(m + ((((rrvv32.reg1val & 0xFF) << 1) + r.ihbase) % RAM_SIZE)) = rrvv32.reg2val;
		break;
	case I_HALT:
		return ERR_HALT;
	case I_RESET:
		cpu_reset();
		break;
	case I_TRAP:
		r.flagr |= FLAG_TRAP;
		break;
	case I_ENCOFF:
		r.flagr &= ~FLAG_ENCON;
		break;
	case I_ENCON:
		r.flagr |= FLAG_ENCON;
		break;
	case I_PUSHREG:
		push64(r.r12);
		push64(r.r34);
		push64(r.r56);
		break;
	case I_POPREG:
		pop64(r.r56);
		pop64(r.r34);
		pop64(r.r12);
		break;
	case I_ENCRETN:
		r.flagr |= FLAG_ENCON;
		r.pc = ipop();
		break;
		// 64-bit Integer Arithmetic
	case I_ADD64:
		*rrvv64.reg1s += rrvv64.reg2vals;
		break;
	case I_SUB64:
		*rrvv64.reg1s -= rrvv64.reg2vals;
		break;
	case I_MUL64:
		*rrvv64.reg1s *= rrvv64.reg2vals;
		break;
	case I_DIV64:
		*rrvv64.reg1s /= rrvv64.reg2vals;
		break;
	case I_MOD64:
		*rrvv64.reg1s %= rrvv64.reg2vals;
		break;
	case I_SHL64:
		*rrvv64.reg1s <<= rrvv64.reg2vals;
		break;
	case I_LSHR64:
		*rrvv64.reg1 >>= rrvv64.reg2val;
		break;
	case I_ASHR64:
		*rrvv64.reg1s >>= rrvv64.reg2vals;
		break;
	case I_MULU64:
		*rrvv64.reg1 *= rrvv64.reg2val;
		break;
	case I_DIVU64:
		*rrvv64.reg1 /= rrvv64.reg2val;
		break;
		// 32-bit Float Arithmetic
	case I_ADDF:
		*rrvv32.reg1f += rrvv32.reg2valf;
		break;
	case I_SUBF:
		*rrvv32.reg1f -= rrvv32.reg2valf;
		break;
	case I_MULF:
		*rrvv32.reg1f *= rrvv32.reg2valf;
		break;
	case I_DIVF:
		*rrvv32.reg1f /= rrvv32.reg2valf;
		break;
		// 64-bit Float Arithmetic
	case I_ADD64F:
		*rrvv64.reg1f += rrvv64.reg2valf;
		break;
	case I_SUB64F:
		*rrvv64.reg1f -= rrvv64.reg2valf;
		break;
	case I_MUL64F:
		*rrvv64.reg1f *= rrvv64.reg2valf;
		break;
	case I_DIV64F:
		*rrvv64.reg1f /= rrvv64.reg2valf;
		break;
	case I_MOV8:
		*(uint8_t*)rrvv32.reg1 = rrvv32.reg2val & 0xFF;
		break;
	case I_MOV16:
		*(uint16_t*)rrvv32.reg1 = rrvv32.reg2val & 0xFFFF;
		break;
		// Extra
	case I_DEBUG:
		//printf("Registers:\nR1=%08x R2=%08x R3=%08x R4=%08x R5=%08x R6=%08x\nPSP=%08x CSP=%08x PC=%08x IHBASE=%08x ENCREG=%016I64x\n", r.r1, r.r2, r.r3, r.r4, r.r5, r.r6, r.psp, r.csp, r.pc, r.ihbase, r.encreg12);
		break;
	case I_SOFF1:
		r.r1offset = siread8();
		break;
	case I_SOFF2:
		r.r2offset = siread8();
		break;
	case I_SOFF12:
		r.r1offset = siread8();
		r.r2offset = siread8();
		break;
	case I_ROFF1:
		r.r1offset = 0;
		break;
	case I_ROFF2:
		r.r2offset = 0;
		break;
	case I_ROFF12:
		r.r1offset = 0;
		r.r2offset = 0;
		break;
	case I_OSOFF1:
		r.r1offset = siread8();
		r.flagr |= FLAG_RSTOFF1;
		break;
	case I_OSOFF2:
		r.r2offset = siread8();
		r.flagr |= FLAG_RSTOFF2;
		break;
	case I_OSOFF12:
		r.r1offset = siread8();
		r.r2offset = siread8();
		r.flagr |= FLAG_RSTOFF12;
		break;
	default:
		return interrupt(INT_ILLEGAL_OPCODE);
	}

	return 0;
}

uint8_t cpu_step() {
	if (cpu_needs_reset) {
		return ERR_HALT;
	}

	uint8_t res = _cpu_step();
	if (!res) {
		if (r.flagr & FLAG_TRAP) {
			r.flagr &= ~FLAG_TRAP;
			uint8_t ires = interrupt(INT_TRAP);
			if (ires) {
				return ires;
			}
		}
		return 0;
	}

	if (res != ERR_HALT && !interrupt_nopush(INT_ERR)) {
		push(res);
		return 0;
	}

	cpu_needs_reset = 1;
	return res;
}

#pragma GCC diagnostic pop
