#ifndef REGISTERS_H_INCLUDED
#define REGISTERS_H_INCLUDED

#include <stdint.h>

#define CREG_ID 13
#define MREG_ID 14
#define MREGC_ID 15

#define FLAG_EQ	(0b1)
#define FLAG_LT	(0b10)
//#define FLAG_GT	(0b100)

#define FLAG_TRAP  (0b1000)
#define FLAG_ENCON (0b10000)

//#define FLAG_LTE   (FLAG_EQ|FLAG_LT)
//#define FLAG_GTE   (FLAG_EQ|FLAG_GT)
#define FLAG_CMP   (FLAG_EQ|FLAG_LT)
#define FLAG_NOCMP (~FLAG_CMP)

#define REGISTERS_SIZE 13

#pragma pack(push, 1)
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
					float r3f;
				};
				union {
					uint32_t r4;
					int32_t r4s;
					float r4f;
				};
			};
		};
		union {
			uint64_t r56;
			int64_t r56s;
			double r56f;
			struct {
				union {
					uint32_t r5;
					int32_t r5s;
					float r5f;
				};
				union {
					uint32_t r6;
					int32_t r6s;
					float r6f;
				};
			};
		};
		uint32_t psp;
		uint32_t csp;
		uint32_t pc;
		union {
			uint32_t msr;
			struct {
				uint8_t flagr;
				int8_t r1offset;
				int8_t r2offset;
				uint8_t unusedmsr;
			};
		};
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
#pragma pack(pop)

registers_t r;

#endif // REGISTERS_H_INCLUDED
