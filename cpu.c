#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "bootloader.h"

#define REGISTERS_SIZE 16
#define RAM_SIZE 65536

#define CREG_ID 6
#define MREG_ID 7
#define MREGC_ID 12

#define FLAG_EQ    (0b1)
#define FLAG_LT    (0b10)
//#define FLAG_GT    (0b100)

#define FLAG_TRAP  (0b1000)
#define FLAG_ENCON (0b10000)

//#define FLAG_LTE   (FLAG_EQ|FLAG_LT)
//#define FLAG_GTE   (FLAG_EQ|FLAG_GT)
#define FLAG_CMP   (FLAG_EQ|FLAG_LT)
#define FLAG_NOCMP (~FLAG_CMP)

enum ITYPE {
    IT_RRVV = 0,
    IT_N,
    IT_RRVV32,
};

static uint8_t ITYPES[256];

typedef union registers_t {
    struct {
        union {
            uint32_t r12;
            int32_t r12s;
            float r12f;
            struct {
                union {
                    uint16_t r1;
                    int16_t r1s;
                };
                union {
                    uint16_t r2;
                    int16_t r2s;
                };
            };
        };
        union {
            uint32_t r34;
            int32_t r34s;
            float r34f;
            struct {
                union {
                    uint16_t r3;
                    int16_t r3s;
                };
                union {
                    uint16_t r4;
                    int16_t r4s;
                };
            };
        };
        uint16_t sp;
        uint16_t pc;
        uint16_t creg; // 6
        uint16_t mreg; // 7
        union {
            uint32_t encreg12;
            struct {
                uint16_t encreg1;
                uint16_t encreg2;
            };
        };
        uint16_t flagr;
        uint16_t ihbase;
        uint16_t mregc;

    };
    uint16_t u[REGISTERS_SIZE];
    int16_t s[REGISTERS_SIZE];
} registers_t;

static registers_t r;

static uint8_t m[RAM_SIZE];

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
    memset(&r, 0, sizeof(r));
    memcpy(m, BOOTLOADER, sizeof(BOOTLOADER));
}

void cpu_init() {
    int i;

    memset(m, 0, RAM_SIZE);
    memset(ITYPES, 0, 256);

    ITYPES[7] = IT_N; // NOP
    ITYPES[47] = IT_N; // RETN
    for (i = 50; i <= 56; i++)
        ITYPES[i] = IT_N; // HALT/RESET/TRAP/ENCOFF/ENCON/PUSHREG/POPREG

    ITYPES[2] = IT_RRVV32; // MOV32
    ITYPES[5] = IT_RRVV32; // PUSH32
    ITYPES[6] = IT_RRVV32; // POP32
    ITYPES[26] = IT_RRVV32; // CMP32
    ITYPES[27] = IT_RRVV32; // CMP32S
    ITYPES[28] = IT_RRVV32; // CMPF
    for (i = 57; i <= 70; i++)
        ITYPES[i] = IT_RRVV32; // INT32 / FLOAT32

    cpu_reset();
}

static uint8_t iread8() {
    uint16_t opc = r.pc++;
    uint8_t raw = m[opc];
    if (r.flagr & FLAG_ENCON) {
        return raw ^ (uint8_t)((r.encreg12 >> (opc % 4)) & 0xFF);
    }
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

#define IFEQ()  if   (r.flags & FLAG_EQ)
#define IFNEQ() if (!(r.flags & FLAG_EQ))

#define IFLT()  if   (r.flags & FLAG_LT)
#define IFGTE() if (!(r.flags & FLAG_LT))

#define IFLTE() if   (r.flags & (FLAG_LT|FLAG_EQ))
#define IFGT()  if (!(r.flags & (FLAG_LT|FLAG_EQ)))

void cpu_step() {
    uint8_t op = iread8();
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
    case 1:
        *rrvv16.reg1 = rrvv16.reg2val;
        break;
    case 2:
        *rrvv32.reg1 = rrvv32.reg2val;
        break;
    case 3:
        push(rrvv16.reg1val);
        break;
    case 4:
        *rrvv16.reg1 = pop();
        break;
    case 5:
        push32(rrvv16.reg1val);
        break;
    case 6:
        *rrvv16.reg1 = pop32();
        break;
    case 7:
        // NOP
        break;
        // 16-bit Arithmetic
    case 8:
        *rrvv16.reg1s += rrvv16.reg2vals;
        break;
    case 9:
        *rrvv16.reg1s -= rrvv16.reg2vals;
        break;
    case 10:
        *rrvv16.reg1s *= rrvv16.reg2vals;
        break;
    case 11:
        *rrvv16.reg1s /= rrvv16.reg2vals;
        break;
    case 12:
        *rrvv16.reg1s %= rrvv16.reg2vals;
        break;
    case 13:
        *rrvv16.reg1s <<= rrvv16.reg2vals;
        break;
    case 14: // LSHR
        *rrvv16.reg1 >>= rrvv16.reg2val;
        break;
    case 15: // ASHR
        *rrvv16.reg1s >>= rrvv16.reg2vals;
        break;
    case 16:
        *rrvv16.reg1 *= rrvv16.reg2val;
        break;
    case 17:
        *rrvv16.reg1 /= rrvv16.reg2val;
        break;
        // Logic
    case 18:
        *rrvv16.reg1 ^= rrvv16.reg2val;
        break;
    case 19:
        *rrvv16.reg1 |= rrvv16.reg2val;
        break;
    case 20:
        *rrvv16.reg1 &= rrvv16.reg2val;
        break;
    case 21:
        *rrvv16.reg1 = ~rrvv16.reg2val;
        break;
    case 22:
        *rrvv16.reg1 |= ~rrvv16.reg2val;
        break;
    case 23:
        *rrvv16.reg1 &= ~rrvv16.reg2val;
        break;
        // Compare
    case 24:
        DOCMP(rrvv16.reg1val, rrvv16.reg2val);
        break;
    case 25:
        DOCMP(rrvv16.reg1vals, rrvv16.reg2vals);
        break;
    case 26:
        DOCMP(rrvv32.reg1val, rrvv32.reg2val);
        break;
    case 27:
        DOCMP(rrvv32.reg1vals, rrvv32.reg2vals);
        break;
    case 28:
        DOCMP(rrvv32.reg1valf, rrvv32.reg2valf);
        break;
    }
}
#pragma GCC diagnostic pop
