#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>

#define RAM_SIZE 65536

uint8_t m[RAM_SIZE + 16]; // Make sure accidental 64-bit writes don't corrupt R

#endif // MEMORY_H_INCLUDED
