#ifndef REGISTERS_H_INCLUDED
#define REGISTERS_H_INCLUDED

#include <stdint.h>

#define CREG_ID 13
#define MREG_ID 14
#define MREGC_ID 15

#define FLAG_EQ    (0b1)
#define FLAG_LT    (0b10)
//#define FLAG_GT    (0b100)

#define FLAG_TRAP  (0b1000)
#define FLAG_ENCON (0b10000)

//#define FLAG_LTE   (FLAG_EQ|FLAG_LT)
//#define FLAG_GTE   (FLAG_EQ|FLAG_GT)
#define FLAG_CMP   (FLAG_EQ|FLAG_LT)
#define FLAG_NOCMP (~FLAG_CMP)

#define REGISTERS_SIZE 16

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
        union {
            uint32_t encreg12;
            struct {
                uint16_t encreg1;
                uint16_t encreg2;
            };
        };
        uint16_t flagr;
        uint16_t ihbase;

    };
    uint16_t u[REGISTERS_SIZE];
    int16_t s[REGISTERS_SIZE];
} registers_t;
static registers_t r;

#endif // REGISTERS_H_INCLUDED
