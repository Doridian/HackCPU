#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define ERR_UNHANDLED_INTERRUPT 1
#define ERR_HALT 2
#define ERR_INVALID_IO 3
#define ERR_INVALID_REGISTER 4

#define INT_IO_WRITE 0
#define INT_IO_READ 1
#define INT_IO_WPTR_SET 2
#define INT_IO_RPTR_SET 3
#define INT_IO_WPTR_GET 4
#define INT_IO_RPTR_GET 5
#define INT_IO_LENGTH_GET 6
#define INT_IO_RESET 7

#define INT_TRAP 130
#define INT_ERR 129
#define INT_ILLEGAL_OPCODE 128

// 4MB RAM
#define RAM_SIZE_DEFAULT (1024 * 1024 * 4)

#include <stdint.h>
#include "io.h"
#include "registers.h"

typedef uint8_t(*cpu_interrupt_handler)(uint8_t interrupt);

typedef struct cpu_state_t {
	registers_t reg;
	cpu_interrupt_handler interrupts[256];

	uint8_t* ram;
	uint32_t ram_size;

	iostream_t** io;
	uint32_t io_size;

	uint32_t id;

	uint64_t instruction_counter;
	uint8_t needs_reset;
} cpu_state_t;

typedef cpu_state_t* cpu_state;

cpu_state cpu_init(uint32_t iocount, uint32_t cpuid, uint32_t ram_size);
void cpu_free(cpu_state s);

void cpu_set_io(cpu_state s, int id, iostream_t iostr);
void cpu_reset(cpu_state s);
uint8_t cpu_run(cpu_state s);
uint8_t cpu_step(cpu_state s);

#endif // CPU_H_INCLUDED
