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
    size_t *ptrw = (size_t *)ptr;
    size_t numw  = (num & -sizeof(size_t)) / sizeof(size_t);
    while (numw--) {
        *ptrw++ = 0;
    }
    num &= (sizeof(size_t) - 1);
    uint8_t *ptrb = (uint8_t *)ptrw;
    while (num--) {
        *ptrb++ = 0;
    }
}

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

typedef struct regregvalval16_t {
    union {
        uint16_t* reg1;
        int16_t* reg1s;
    };
    union {
        uint16_t* reg2;
        int16_t* reg2s;
    };
    union {
        uint16_t reg1val;
        int16_t reg1vals;
    };
    union {
        uint16_t reg2val;
        int16_t reg2vals;
    };
} regregvalval16_t;

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

uint8_t dummyrom[] = { 0x00, 0x00, 0x00, 0x00, I_HALT };

static uint8_t dummyrom_read(struct iostream_t* io) {
    return dummyrom[io->rptr];
}

void cpu_init() {
    memclear(m, RAM_SIZE);

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
    io[IO_ROM].length = 5;
    io[IO_ROM].read = dummyrom_read;
    io[IO_ROM].write = 0;
    io[IO_ROM].flags = FLAG_RPTR_GET|FLAG_RPTR_SET|FLAG_LENGTH|FLAG_RESET;

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
    io[3].flags = FLAG_WPTR_GET|FLAG_WPTR_SET|FLAG_RPTR_GET|FLAG_RPTR_SET|FLAG_LENGTH|FLAG_RESET;

    cpu_reset();
}

static uint8_t iread8() {
    uint16_t opc = r.pc++;
    uint8_t raw = m[opc];
    //if (r.flagr & FLAG_ENCON) {
    //    return raw ^ (uint8_t)((r.encreg12 >> (opc % 4)) & 0xFF);
    //}
    return raw;
}

static uint16_t iread16() {
    return (iread8() << 8) + iread8();
}

static uint32_t iread32() {
    return (iread8() << 24) + (iread8() << 16) + (iread8() << 8) + iread8();
}

static regregvalval16_t ireadrrvv16() {
    uint8_t regs = iread8();
    uint8_t r1 = (regs >> 4) & 0x0F;
    uint8_t r2 = regs & 0x0F;
    regregvalval16_t res;
    res.reg1 = r.u + r1; // TODO: Make sure this adds 2 * res.reg1, not 1!
    res.reg2 = r.u + r2; // TODO: Make sure this adds 2 * res.reg2, not 1!
    if (r1 == CREG_ID) {
        res.reg1 = (uint16_t*)(m + r.pc);
        res.reg1val = iread16();
    } else if (r1 == MREG_ID) {
        res.reg1 = (uint16_t*)(m + r.u[iread8() & 0x0F]);
        res.reg1val = *res.reg1;
    } else if (r1 == MREGC_ID) {
        res.reg1 = (uint16_t*)(m + iread16());
        res.reg1val = *(uint16_t*)res.reg1;
    } else {
        res.reg1val = *res.reg1;
    }
    if (r2 == CREG_ID) {
        res.reg2 = (uint16_t*)(m + r.pc);
        res.reg2val = iread16();
    } else if (r2 == MREG_ID) {
        res.reg2 = (uint16_t*)(m + r.u[iread8() & 0x0F]);
        res.reg2val = *res.reg2;
    } else if (r2 == MREGC_ID) {
        res.reg2 = (uint16_t*)(m + iread16());
        res.reg2val = *(uint16_t*)res.reg2;
    } else {
        res.reg2val = *res.reg2;
    }
    return res;
}

static regregvalval32_t ireadrrvv32() {
    uint8_t regs = iread8();
    uint8_t r1 = (regs >> 4) & 0x0F;
    uint8_t r2 = regs & 0x0F;
    regregvalval32_t res;
    res.reg1 = (uint32_t*)(r.u + r1); // TODO: Make sure this adds 2 * res.reg1, not 1!
    res.reg2 = (uint32_t*)(r.u + r2); // TODO: Make sure this adds 2 * res.reg2, not 1!
    if (r1 == CREG_ID) {
        res.reg1 = (uint32_t*)(m + r.pc);
        res.reg1val = iread32();
    } else if (r1 == MREG_ID) {
        res.reg1 = (uint32_t*)(m + r.u[iread8() & 0x0F]);
        res.reg1val = *(uint32_t*)res.reg1;
    } else if (r1 == MREGC_ID) {
        res.reg1 = (uint32_t*)(m + iread16());
        res.reg1val = *(uint32_t*)res.reg1;
    } else {
        res.reg1val = *(uint32_t*)res.reg1;
    }
    if (r2 == CREG_ID) {
        res.reg2 = (uint32_t*)(m + r.pc);
        res.reg2val = iread32();
    } else if (r2 == MREG_ID) {
        res.reg2 = (uint32_t*)(m + r.u[iread8() & 0x0F]);
        res.reg2val = *(uint32_t*)res.reg2;
    } else if (r2 == MREGC_ID) {
        res.reg2 = (uint32_t*)(m + iread16());
        res.reg2val = *(uint32_t*)res.reg2;
    } else {
        res.reg2val = *(uint32_t*)res.reg2;
    }
    return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

static void push(uint16_t i) {
    uint16_t* m16 = (uint16_t*)(m + r.sp);
    r.sp += 2;
    *m16 = i;
}

static void push32(uint32_t i) {
    uint32_t* m32 = (uint32_t*)(m + r.sp);
    r.sp += 4;
    *m32 = i;
}

static uint16_t pop() {
    r.sp -= 2;
    return *(uint16_t*)(m + r.sp);
}

static uint32_t pop32() {
    r.sp -= 4;
    return *(uint32_t*)(m + r.sp);
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

#define DOJMP()    DOJMPP(1);
#define DOJMPZ()   DOJMPP(2);
#define DOJMPP(a)  { r.pc = rrvv16.reg ## a ## val; }
#define DOCALL()   DOCALLP(1);
#define DOCALLZ()  DOCALLP(2);
#define DOCALLP(a) { push(r.pc); DOJMPP(a); }

#define IFZ()  if (rrvv16.reg1val == 0)
#define IFNZ() if (rrvv16.reg1val != 0)

static uint8_t cpu_interrupt(uint8_t i) {
    if (i > 7) {
        return ERR_UNHANDLED_INTERRUPT;
    }
    uint16_t ioid = pop();
    if (ioid >= IOSTREAM_COUNT) {
        return ERR_INVALID_IO;
    }
    iostream_t iostr = io[ioid];
    uint16_t offset, len, x;

    switch (i) {
    case 0:
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
    case 1:
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
    case 2:
        if (iostr.flags | FLAG_WPTR_SET) {
            iostr.wptr = pop32();
        } else {
            return ERR_INVALID_IO;
        }
        break;
    case 3:
        if (iostr.flags | FLAG_RPTR_SET) {
            iostr.rptr = pop32();
        } else {
            return ERR_INVALID_IO;
        }
        break;
    case 4:
        if (iostr.flags | FLAG_WPTR_GET) {
            push32(iostr.wptr);
        } else {
            return ERR_INVALID_IO;
        }
        break;
    case 5:
        if (iostr.flags | FLAG_RPTR_GET) {
            push32(iostr.rptr);
        } else {
            return ERR_INVALID_IO;
        }
        break;
    case 6:
        if (iostr.flags | FLAG_LENGTH) {
            push32(iostr.length);
        } else {
            return ERR_INVALID_IO;
        }
        break;
    case 7:
        if (iostr.flags | FLAG_RESET) {
            iostr.wptr = 0;
            iostr.rptr = 0;
        } else {
            return ERR_INVALID_IO;
        }
        break;
    }
    return 0;
}

static uint8_t interrupt(uint8_t i, int32_t info) {
    if (r.ihbase == 0) {
        return cpu_interrupt(i);
    }
    uint16_t newpc = *(uint16_t*)(m + (i << 1) + r.ihbase);
    if (newpc == 0) {
        return cpu_interrupt(i);
    }
    if (i & 0b10000000) {
        push(r.pc);
        if (info >= 0) {
            push(info);
        }
    }
    r.pc = newpc;
    return 0;
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
    if (r.pc > 1024) {
        cpu_needs_reset = 1;
        return ERR_HALT;
    }
    regregvalval16_t rrvv16;
    regregvalval32_t rrvv32;

    switch (ITYPES[op]) {
    case IT_RRVV:
        rrvv16 = ireadrrvv16();
        break;
    case IT_RRVV32:
        rrvv32 = ireadrrvv32();
        break;
    default:
    case IT_N:
        break;
    }

    switch (op) {
        // Basic
    case I_MOV:
        *rrvv16.reg1 = rrvv16.reg2val;
        break;
    case I_MOV32:
        *rrvv32.reg1 = rrvv32.reg2val;
        break;
    case I_PUSH:
        push(rrvv16.reg1val);
        break;
    case I_POP:
        *rrvv16.reg1 = pop();
        break;
    case I_PUSH32:
        push32(rrvv16.reg1val);
        break;
    case I_POP32:
        *rrvv16.reg1 = pop32();
        break;
    case I_NOP:
        // NOP
        break;
        // 16-bit Arithmetic
    case I_ADD:
        *rrvv16.reg1s += rrvv16.reg2vals;
        break;
    case I_SUB:
        *rrvv16.reg1s -= rrvv16.reg2vals;
        break;
    case I_MUL:
        *rrvv16.reg1s *= rrvv16.reg2vals;
        break;
    case I_DIV:
        *rrvv16.reg1s /= rrvv16.reg2vals;
        break;
    case I_MOD:
        *rrvv16.reg1s %= rrvv16.reg2vals;
        break;
    case I_SHL:
        *rrvv16.reg1s <<= rrvv16.reg2vals;
        break;
    case I_LSHR:
        *rrvv16.reg1 >>= rrvv16.reg2val;
        break;
    case I_ASHR:
        *rrvv16.reg1s >>= rrvv16.reg2vals;
        break;
    case I_MULU:
        *rrvv16.reg1 *= rrvv16.reg2val;
        break;
    case I_DIVU:
        *rrvv16.reg1 /= rrvv16.reg2val;
        break;
        // Logic
    case I_XOR:
        *rrvv16.reg1 ^= rrvv16.reg2val;
        break;
    case I_OR:
        *rrvv16.reg1 |= rrvv16.reg2val;
        break;
    case I_AND:
        *rrvv16.reg1 &= rrvv16.reg2val;
        break;
    case I_NOT:
        *rrvv16.reg1 = ~rrvv16.reg2val;
        break;
    case I_NOR:
        *rrvv16.reg1 |= ~rrvv16.reg2val;
        break;
    case I_NAND:
        *rrvv16.reg1 &= ~rrvv16.reg2val;
        break;
        // Compare
    case I_CMP:
        DOCMP(rrvv16.reg1val, rrvv16.reg2val);
        break;
    case I_CMPS:
        DOCMP(rrvv16.reg1vals, rrvv16.reg2vals);
        break;
    case I_CMP32:
        DOCMP(rrvv32.reg1val, rrvv32.reg2val);
        break;
    case I_CMP32S:
        DOCMP(rrvv32.reg1vals, rrvv32.reg2vals);
        break;
    case I_CMPF:
        DOCMP(rrvv32.reg1valf, rrvv32.reg2valf);
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
        r.pc = pop();
        break;
        // Special
    case I_INT:
        return interrupt(rrvv16.reg1val, -1);
    case I_SETIH:
        m[((rrvv16.reg1val & 0xFF) << 1) + r.ihbase] = rrvv16.reg2val;
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
        r.flagr |= FLAG_ENCON;
        break;
    case I_ENCON:
        r.flagr &= ~FLAG_ENCON;
        break;
    case I_PUSHREG:
        push32(r.r12);
        push32(r.r34);
        break;
    case I_POPREG:
        pop32(r.r34);
        pop32(r.r12);
        break;
    case I_ENCRETN:
        r.flagr |= FLAG_ENCON;
        r.pc = pop();
        break;
        // 32-bit Integer Arithmetic
    case I_ADD32:
        *rrvv32.reg1s += rrvv32.reg2vals;
        break;
    case I_SUB32:
        *rrvv32.reg1s -= rrvv32.reg2vals;
        break;
    case I_MUL32:
        *rrvv32.reg1s *= rrvv32.reg2vals;
        break;
    case I_DIV32:
        *rrvv32.reg1s /= rrvv32.reg2vals;
        break;
    case I_MOD32:
        *rrvv32.reg1s %= rrvv32.reg2vals;
        break;
    case I_SHL32:
        *rrvv32.reg1s <<= rrvv32.reg2vals;
        break;
    case I_LSHR32:
        *rrvv32.reg1 >>= rrvv32.reg2val;
        break;
    case I_ASHR32:
        *rrvv32.reg1s >>= rrvv32.reg2vals;
        break;
    case I_MULU32:
        *rrvv32.reg1 *= rrvv32.reg2val;
        break;
    case I_DIVU32:
        *rrvv32.reg1 /= rrvv32.reg2val;
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
    default:
         return interrupt(128, op);
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
            uint8_t ires = interrupt(130, -1);
            if (ires) {
                return ires;
            }
        }
        return 0;
    }
    if (!interrupt(129, res)) {
        return 0;
    }

    cpu_needs_reset = 1;
    return res;
}

#pragma GCC diagnostic pop
