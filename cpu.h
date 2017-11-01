#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdint.h>

void cpu_init();
void cpu_reset();
void cpu_run();
uint8_t cpu_step();

#endif // CPU_H_INCLUDED
