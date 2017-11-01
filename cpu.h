#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define ERR_UNHANDLED_INTERRUPT 1
#define ERR_HALT 2

#include <stdint.h>

void cpu_init();
void cpu_reset();
uint8_t cpu_run();
uint8_t cpu_step();

#endif // CPU_H_INCLUDED
