#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#define IO_STDOUT 0
#define IO_ROM    1
#define IO_STDIN  2
#define IO_ZERO   3

#define IO_FLAG_WPTR_GET 0b1
#define IO_FLAG_WPTR_SET 0b10
#define IO_FLAG_RPTR_GET 0b100
#define IO_FLAG_RPTR_SET 0b1000
#define IO_FLAG_RESET	  0b10000
#define IO_FLAG_LENGTH   0b100000

typedef struct iostream_t {
	uint32_t rptr;
	uint32_t wptr;
	uint32_t length;
	uint8_t flags;
	uint8_t (*read)(uint32_t, struct iostream_t*);
	void (*write)(uint32_t, struct iostream_t*, uint8_t);

	void *userdata;
} iostream_t;

#endif // IO_H_INCLUDED
