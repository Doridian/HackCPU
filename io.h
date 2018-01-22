#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#define IOSTREAM_COUNT 5

#define IO_STDOUT 0
#define IO_ROM 1
#define IO_STDIN 2

#define FLAG_WPTR_GET 0b1
#define FLAG_WPTR_SET 0b10
#define FLAG_RPTR_GET 0b100
#define FLAG_RPTR_SET 0b1000
#define FLAG_RESET	0b10000
#define FLAG_LENGTH   0b100000

typedef struct iostream_t {
	uint32_t rptr;
	uint32_t wptr;
	uint32_t length;
	uint8_t flags;
	uint8_t (*read)(struct iostream_t*);
	void (*write)(struct iostream_t*, uint8_t);
} iostream_t;

iostream_t io[IOSTREAM_COUNT];

#endif // IO_H_INCLUDED
