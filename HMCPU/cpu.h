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

#include <stdint.h>

uint64_t cpu_instruction_counter;

typedef uint8_t(*cpu_interrupt_handler)(uint8_t interrupt);
cpu_interrupt_handler cpu_interrupts[256];

void cpu_init();
void cpu_reset();
uint8_t cpu_run();
uint8_t cpu_step();

#endif // CPU_H_INCLUDED
