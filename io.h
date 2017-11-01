#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

typedef struct iostream_t {
    uint64_t rptr;
    uint64_t wptr;
    uint64_t length;
    uint8_t (*read)(struct iostream_t*);
    void (*write)(struct iostream_t*, uint8_t);
} iostream_t;

#endif // IO_H_INCLUDED
