#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdlib.h>
#define mem_alloc malloc
#define mem_free free
#define mem_copy memcpy
#define mem_clear(ptr, len) memset(ptr, 0, len)

#endif
