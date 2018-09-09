#include <stdint.h>
#include <stddef.h>
#include "cpu.h"
#include "registers.h"
#include "memory.h"
#include "opcodes.h"
#include "bootloader.h"
#include "io.h"

#define REG_FLAG_OFFSET 0b10000

void cpu_reset(cpu_state s) {
	mem_clear(&s->r, sizeof(s->r));
	int i;
	int32_t bootloader_baseaddr = *(int32_t*)(BOOTLOADER + sizeof(BOOTLOADER) - 4);
	if (bootloader_baseaddr < 0) {
		bootloader_baseaddr = s->ram_size + bootloader_baseaddr;
		bootloader_baseaddr -= bootloader_baseaddr % 8;
	}
	for (i = 0; i < sizeof(BOOTLOADER) - 4; i++) {
		s->m[i + bootloader_baseaddr] = BOOTLOADER[i];
	}
	s->r.pc = bootloader_baseaddr;
	s->_needs_reset = 0;
	s->instruction_counter = 0;
}

static void devzero_write(uint32_t id, iostream_t* io, uint8_t i) { }
static uint8_t devzero_read(uint32_t id, iostream_t* io) { return 0; }

uint8_t dummyrom[1] = { 0x00 };

static uint8_t dummyrom_read(uint32_t id, iostream_t* io) {
	return dummyrom[io->rptr];
}

void cpu_set_io(cpu_state s, int id, iostream_t iostr) {
	iostream_t* subio = mem_alloc(sizeof(iostream_t));
	mem_copy(subio, &iostr, sizeof(iostream_t));
	s->io[id] = subio;
}

cpu_state cpu_init(uint32_t iocount, uint32_t cpuid, uint32_t ram_size) {
	cpu_state s = mem_alloc(sizeof(cpu_state_t));

	if (iocount < 4) {
		iocount = 4;
	}
	s->iocount = iocount;
	s->io = mem_alloc(sizeof(iostream_t*) * iocount);
	s->id = cpuid;

	s->ram_size = ram_size;
	s->m = mem_alloc(ram_size + 16); // Make sure accidental 64-bit writes don't corrupt memory

	//mem_clear(m, RAM_SIZE);
	mem_clear(s->interrupts, sizeof(s->interrupts));

	iostream_t mkio;
	// stdout
	mkio.rptr = 0;
	mkio.wptr = 0;
	mkio.length = 0;
	mkio.read = 0;
	mkio.write = devzero_write;
	mkio.flags = 0;
	cpu_set_io(s, IO_STDOUT, mkio);

	// ROM
	mkio.rptr = 0;
	mkio.wptr = 0;
	mkio.length = sizeof(dummyrom);
	mkio.read = dummyrom_read;
	mkio.write = 0;
	mkio.flags = IO_FLAG_RPTR_GET|IO_FLAG_RPTR_SET|IO_FLAG_LENGTH|IO_FLAG_RESET;
	cpu_set_io(s, IO_ROM, mkio);

	// stdin
	mkio.rptr = 0;
	mkio.wptr = 0;
	mkio.length = 0;
	mkio.read = devzero_read;
	mkio.write = 0;
	mkio.flags = 0;
	cpu_set_io(s, IO_STDIN, mkio);

	// /dev/zero
	mkio.rptr = 0;
	mkio.wptr = 0;
	mkio.length = 0;
	mkio.read = devzero_read;
	mkio.write = devzero_write;
	mkio.flags = IO_FLAG_WPTR_GET|IO_FLAG_WPTR_SET|IO_FLAG_RPTR_GET|IO_FLAG_RPTR_SET|IO_FLAG_LENGTH|IO_FLAG_RESET;
	cpu_set_io(s, IO_ZERO, mkio);

	cpu_reset(s);

	return s;
}

void cpu_free(cpu_state s) {
	if (s->m != NULL) {
		mem_free(s->m);
		s->m = NULL;
	}

	if (s->io != NULL) {
		for (uint32_t i = 0; i < s->iocount; i++) {
			iostream_t* io = s->io[i];
			if (io != NULL) {
				mem_free(io);
			}
		}

		mem_free(s->io);
		s->io = NULL;
	}

	mem_free(s);
}

static inline uint8_t iread8(cpu_state s) {
	uint32_t opc = s->r.pc++;
	if (opc >= s->ram_size) {
		s->r.pc = 1;
		opc = 0;
	}
	uint8_t raw = s->m[opc];
	if (s->r.flagr & FLAG_ENCON) {
		return raw ^ (uint8_t)((s->r.encreg12 >> ((opc % 8) << 3)) & 0xFF);
	}
	return raw;
}

static inline int8_t siread8(cpu_state s) {
	uint8_t u8 = iread8(s);
	return *(int8_t*)&u8;
}

static inline int8_t siread8_no0(cpu_state s) {
	int8_t i8 = siread8(s);
	if (i8 >= 0) {
		return i8 + 1;
	}
	return i8;
}

#define __iread8_64(s) ((uint64_t)iread8(s))
#define __iread8_32(s) ((uint32_t)iread8(s))

static inline uint32_t constrain_ram(cpu_state s, uint32_t val) {
	return (val % s->ram_size);
}

static inline uint32_t iread32(cpu_state s) {
	return __iread8_32(s) + (__iread8_32(s) << 8) + (__iread8_32(s) << 16) + (__iread8_32(s) << 24);
}

static inline uint64_t iread64(cpu_state s) {
	return __iread8_64(s) + (__iread8_64(s) << 8) + (__iread8_64(s) << 16) + (__iread8_64(s) << 24) + (__iread8_64(s) << 32) + (__iread8_64(s) << 40) + (__iread8_64(s) << 48) + (__iread8_64(s) << 56);
}

#define ireadrv(s, nbits, idx) res.reg##idx = (uint##nbits##_t*)(s->r.u + r##idx); \
	if (r##idx == CREG_ID) { \
		res.reg##idx = (uint##nbits##_t*)(s->m + constrain_ram(s, s->r.pc)); \
		res.reg##idx##val = iread##nbits(s); \
	} else if (r##idx == MREG_ID) { \
		uint8_t regid = iread8(s); \
		int8_t offset = (regid & REG_FLAG_OFFSET) ? siread8_no0(s) : 0; \
		res.reg##idx = (uint##nbits##_t*)(s->m + constrain_ram(s, s->r.u[regid & 0x0F] + offset)); \
		res.reg##idx##val = *res.reg##idx; \
	} else if (r##idx == MREGC_ID) { \
		res.reg##idx = (uint##nbits##_t*)(s->m + constrain_ram(s, iread32(s))); \
		res.reg##idx##val = *(uint##nbits##_t*)res.reg##idx; \
	} else if (r##idx >= REGISTERS_SIZE) { \
		res.reg##idx = NULL; \
		res.reg##idx##val = 0; \
	} else { \
		res.reg##idx##val = *res.reg##idx; \
	}

#define rrvvNN(nbits, ftype, idx) \
	union { \
		uint##nbits##_t* reg##idx; \
		int##nbits##_t* reg##idx##s; \
		ftype* reg##idx##f; \
	}; \
	union { \
		uint##nbits##_t reg##idx##val; \
		int##nbits##_t reg##idx##vals; \
		ftype reg##idx##valf; \
	};

#define ireadrrvvN(nbits, ftype) \
typedef struct regregvalval##nbits##_t { \
	rrvvNN(nbits, ftype, 1) \
	rrvvNN(nbits, ftype, 2) \
} regregvalval##nbits##_t; \
\
static inline regregvalval##nbits##_t ireadrrvv##nbits(cpu_state s) { \
	uint8_t regs = iread8(s); \
	uint8_t r1 = (regs >> 4) & 0x0F; \
	uint8_t r2 = regs & 0x0F; \
	regregvalval##nbits##_t res; \
	ireadrv(s, nbits, 1); \
	ireadrv(s, nbits, 2); \
	return res; \
}

ireadrrvvN(32, float)
ireadrrvvN(64, double)

static void push(cpu_state s, uint32_t i) {
	s->r.csp -= 4;
	uint32_t* m32 = (uint32_t*)(s->m + constrain_ram(s, s->r.csp));
	*m32 = i;
}

static void push64(cpu_state s, uint64_t i) {
	s->r.csp -= 8;
	uint64_t* m64 = (uint64_t*)(s->m + constrain_ram(s, s->r.csp));
	*m64 = i;
}

static uint32_t pop(cpu_state s) {
	uint32_t res = *(uint32_t*)(s->m + constrain_ram(s, s->r.csp));
	s->r.csp += 4;
	return res;
}

static uint64_t pop64(cpu_state s) {
	uint64_t res = *(uint64_t*)(s->m + constrain_ram(s, s->r.csp));
	s->r.csp += 8;
	return res;
}

#define DOCMP(a, b) s->r.flagr = (s->r.flagr & FLAG_NOCMP) | \
						  	    ((a < b) << 1) | \
							    (a == b);

#define IFEQ()  if   (s->r.flagr & FLAG_EQ)
#define IFNEQ() if (!(s->r.flagr & FLAG_EQ))

#define IFLT()  if   (s->r.flagr & FLAG_LT)
#define IFGTE() if (!(s->r.flagr & FLAG_LT))

#define IFLTE() if   (s->r.flagr & (FLAG_LT|FLAG_EQ))
#define IFGT()  if (!(s->r.flagr & (FLAG_LT|FLAG_EQ)))

#define PUSHCALLSTACK() { \
	push(s, s->r.pc); \
	push(s, s->r.bsp); \
	s->r.bsp = s->r.csp; \
}
#define DOJMP()	   DOJMPP(1);
#define DOJMPZ()   DOJMPP(2);
#define DOJMPP(a)  { s->r.pc = rrvv32.reg ## a ## val; }
#define DOCALL()   DOCALLP(1);
#define DOCALLZ()  DOCALLP(2);
#define DOCALLP(a) { \
	PUSHCALLSTACK(); \
	DOJMPP(a); \
}

#define DORETN() { \
	s->r.csp = s->r.bsp; \
	s->r.bsp = pop(s); \
	s->r.pc = pop(s); \
}

#define IFZ()  if (rrvv32.reg1val == 0)
#define IFNZ() if (rrvv32.reg1val != 0)

static uint8_t cpu_interrupt(cpu_state s, uint8_t i) {
	if (i > 7) {
		cpu_interrupt_handler custom = s->interrupts[i];
		if (custom) {
			return custom(i);
		}
		return ERR_UNHANDLED_INTERRUPT;
	}

	uint32_t ioid = pop(s);
	if (ioid >= s->iocount) {
		return ERR_INVALID_IO;
	}
	iostream_t* iostr = s->io[ioid];
	if (iostr == NULL) {
		return ERR_INVALID_IO;
	}
	uint32_t offset, len, x;

	switch (i) {
	case INT_IO_WRITE:
		offset = pop(s);
		len = pop(s);
		if (!iostr->write) {
			return ERR_INVALID_IO;
		}
		for (x = 0; x < len; x++) {
			if (x + offset >= s->ram_size) {
				return ERR_INVALID_IO;
			}
			iostr->write(s->id, iostr, s->m[x + offset]);
			iostr->wptr++;
		}
		break;
	case INT_IO_READ:
		offset = pop(s);
		len = pop(s);
		if (!iostr->read) {
			return ERR_INVALID_IO;
		}
		for (x = 0; x < len; x++) {
			if (x + offset >= s->ram_size) {
				return ERR_INVALID_IO;
			}
			s->m[x + offset] = iostr->read(s->id, iostr);
			iostr->rptr++;
		}
		break;
	case INT_IO_WPTR_SET:
		if (iostr->flags | IO_FLAG_WPTR_SET) {
			iostr->wptr = pop(s);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RPTR_SET:
		if (iostr->flags | IO_FLAG_RPTR_SET) {
			iostr->rptr = pop(s);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_WPTR_GET:
		if (iostr->flags | IO_FLAG_WPTR_GET) {
			push(s, iostr->wptr);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RPTR_GET:
		if (iostr->flags | IO_FLAG_RPTR_GET) {
			push(s, iostr->rptr);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_LENGTH_GET:
		if (iostr->flags | IO_FLAG_LENGTH) {
			push(s, iostr->length);
		} else {
			return ERR_INVALID_IO;
		}
		break;
	case INT_IO_RESET:
		if (iostr->flags | IO_FLAG_RESET) {
			iostr->wptr = 0;
			iostr->rptr = 0;
		} else {
			return ERR_INVALID_IO;
		}
		break;
	}
	return 0;
}

static uint8_t interrupt_nopush(cpu_state s, uint8_t i) {
	if (s->r.ihbase == 0) {
		return cpu_interrupt(s, i);
	}
	uint32_t newpc = *(uint32_t*)(s->m + constrain_ram(s, (i << 1) + s->r.ihbase));
	if (newpc == 0) {
		return cpu_interrupt(s, i);
	}
	PUSHCALLSTACK();
	s->r.pc = newpc;
	return 0;
}

static uint8_t interrupt(cpu_state s, uint8_t i) {
	uint8_t res = interrupt_nopush(s, i);
	if (res == ERR_UNHANDLED_INTERRUPT) {
		push(s, i);
	}
	return res;
}

uint8_t cpu_run(cpu_state s) {
	while(1) {
		uint8_t res = cpu_step(s);
		if (res) {
			return res;
		}
	}
}

static uint8_t _cpu_step(cpu_state s) {
	s->instruction_counter++;
	uint8_t op = iread8(s);
	if (op >= I_FIRST_INVALID) {
		return interrupt(s, INT_ILLEGAL_OPCODE);
	}

	//printf("OP = %d\n", op);

	uint8_t tmp8;

	regregvalval32_t rrvv32;
	regregvalval64_t rrvv64;

	switch (ITYPES[op]) {
	case IT_RRVV:
		rrvv32 = ireadrrvv32(s);
		if (rrvv32.reg1 == NULL || rrvv32.reg2 == NULL) {
			return ERR_INVALID_REGISTER;
		}
		break;
	case IT_RRVV64:
		rrvv64 = ireadrrvv64(s);
		if (rrvv64.reg1 == NULL || rrvv64.reg2 == NULL) {
			return ERR_INVALID_REGISTER;
		}
		break;
	// All other cases are handled by the instruction itself
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
		push(s, rrvv32.reg1val);
		break;
	case I_POP:
		*rrvv32.reg1 = pop(s);
		break;
	case I_PUSH64:
		push64(s, rrvv32.reg1val);
		break;
	case I_POP64:
		*rrvv64.reg1 = pop64(s);
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
		DORETN();
		break;
		// Special
	case I_INT:
		return interrupt(s, rrvv32.reg1val);
	case I_SETIH:
		*(uint16_t*)(s->m + constrain_ram(s, ((rrvv32.reg1val & 0xFF) << 1) + s->r.ihbase)) = rrvv32.reg2val;
		break;
	case I_HALT:
		return ERR_HALT;
	case I_RESET:
		cpu_reset(s);
		break;
	case I_TRAP:
		s->r.flagr |= FLAG_TRAP;
		break;
	case I_ENCOFF:
		s->r.flagr &= ~FLAG_ENCON;
		break;
	case I_ENCON:
		s->r.flagr |= FLAG_ENCON;
		break;
	case I_PUSHREG:
		push64(s, s->r.r12);
		push64(s, s->r.r34);
		push64(s, s->r.r56);
		break;
	case I_POPREG:
		s->r.r56 = pop64(s);
		s->r.r34 = pop64(s);
		s->r.r12 = pop64(s);
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
	case I_RETNA:
		DORETN();
		s->r.csp += rrvv32.reg1val;
		break;
	case I_POPNIL:
		s->r.csp += 4;
		break;
	case I_POPNIL64:
		s->r.csp += 8;
		break;
	case I_RETNAC:
		tmp8 = iread8(s);
		DORETN();
		s->r.csp += tmp8;
		break;
	case I_CPUID:
		*rrvv32.reg1 = s->id;
		break;
	case I_RAMSIZE:
		*rrvv32.reg1 = s->ram_size;
		break;
	default:
		return interrupt(s, INT_ILLEGAL_OPCODE);
	}

	return 0;
}

uint8_t cpu_step(cpu_state s) {
	if (s->_needs_reset) {
		return ERR_HALT;
	}

	uint8_t res = _cpu_step(s);
	if (!res) {
		if (s->r.flagr & FLAG_TRAP) {
			s->r.flagr &= ~FLAG_TRAP;
			uint8_t ires = interrupt(s, INT_TRAP);
			if (ires) {
				return ires;
			}
		}
		return 0;
	}

	if (res != ERR_HALT && !interrupt_nopush(s, INT_ERR)) {
		push(s, res);
		return 0;
	}

	s->_needs_reset = 1;
	return res;
}

