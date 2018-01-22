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

#define REGISTERS_SIZE 11

typedef union registers_t {
    struct {
        union {
            uint64_t r12;
            int64_t r12s;
            double r12f;
            struct {
                union {
                    uint32_t r1;
                    int32_t r1s;
                    float r1f;
                };
                union {
                    uint32_t r2;
                    int32_t r2s;
                    float r2f;
                };
            };
        };
        union {
            uint64_t r34;
            int64_t r34s;
            double r34f;
            struct {
                union {
                    uint32_t r3;
                    int32_t r3s;
                };
                union {
                    uint32_t r4;
                    int32_t r4s;
                };
            };
        };
        uint32_t psp;
        uint32_t csp;
        uint32_t pc;
        uint32_t flagr;
        union {
            uint64_t encreg12;
            struct {
                uint32_t encreg1;
                uint32_t encreg2;
            };
        };
        uint32_t ihbase;
    };
    uint32_t u[REGISTERS_SIZE];
    int32_t s[REGISTERS_SIZE];
} registers_t;

registers_t r;

#endif // REGISTERS_H_INCLUDED
