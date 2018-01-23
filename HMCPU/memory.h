#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>

// 4MB RAM
#define RAM_SIZE (1024 * 1024 * 4)

uint8_t m[RAM_SIZE + 16]; // Make sure accidental 64-bit writes don't corrupt memory

#endif // MEMORY_H_INCLUDED
