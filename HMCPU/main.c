#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include "registers.h"
#include "io.h"
#include "memory.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#ifdef _WIN32
#include <Windows.h>
static uint64_t get_highres_timestamp() {
	uint64_t time;
	QueryPerformanceCounter((LARGE_INTEGER *)&time);
	return time;
}

static double get_milliseconds(uint64_t start, uint64_t end) {
	uint64_t freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	double dFreq = 1.0 / freq;

	return (end - start) * dFreq * 1000.0;
}
#endif

static void stdout_write(struct iostream_t* io, uint8_t i) {
	putc(i, stdout);
}

static uint8_t stdin_read(struct iostream_t* io) {
	return getc(stdin);
}

static uint8_t* fhrom_data;
static uint8_t fhrom_read(struct iostream_t* io) {
	return fhrom_data[io->rptr];
}

#define HEXDUMP_SIZE 12

int main(int argc, const char **argv) {
	cpu_init();

	if (argc > 1) {
		const char *romfname = argv[1];
		FILE *romfh;
#ifdef _MSC_VER
		if (fopen_s(&romfh, romfname, "rb") != 0) {
			return 5;
		}
#else
		romfh = fopen(romfname, "rb");
		if (romfh == NULL) {
			return 5;
		}
#endif
		fseek(romfh, 0L, SEEK_END);
		size_t romlen = ftell(romfh);
		io[IO_ROM].length = (uint32_t)romlen;
		io[IO_ROM].read = fhrom_read;
		rewind(romfh);
		fhrom_data = (uint8_t*)malloc(romlen);
		fread(fhrom_data, romlen, 1, romfh);
		fclose(romfh);
	}
	io[IO_STDOUT].write = stdout_write;
	io[IO_STDIN].read = stdin_read;

	cpu_reset();

	uint64_t start_time = get_highres_timestamp();
	uint8_t res = cpu_run();
	uint64_t end_time = get_highres_timestamp();
	//if (res == ERR_HALT) {
	//	return 0;
	//}
	printf("\n\n\nCPU exit code: %02x", res);

	uint32_t opc = r.pc - 1;
	if (r.pc == 0) {
		opc = RAM_SIZE - 1;
	}

	uint8_t intnum;
	switch (res) {
	case ERR_HALT:
		printf(" (Normal HALT)\n");
		break;
	case ERR_INVALID_IO:
		printf(" (Invalid I/O)\n");
		break;
	case ERR_INVALID_REGISTER:
		printf(" (Invalid register access)\n");
		break;
	case ERR_UNHANDLED_INTERRUPT:
		if (r.csp > 2 && r.csp <= RAM_SIZE) {
			intnum = m[r.csp - 2];
		} else {
			intnum = 0xFF;
		}
		printf(" (Unhandled interrupt %02x", intnum);
		switch (intnum) {
		case INT_ERR:
			printf(" (CPU Error)");
			break;
		case INT_ILLEGAL_OPCODE:
			printf(" (Illegal opcode %02x)", m[opc]);
			break;
		case INT_TRAP:
			printf(" (TRAP)");
			break;
		}
		printf(")\n");
		break;
	default:
		printf("\n");
	}

	double time_taken = get_milliseconds(start_time, end_time);
	double ops_per_time = (cpu_instruction_counter / time_taken) * 1000.0;
	char ops_unit = ' ';
	if (ops_per_time >= 1000000000) {
		ops_unit = 'G';
		ops_per_time /= 1000000000;
	}
	else if (ops_per_time >= 1000000) {
		ops_unit = 'M';
		ops_per_time /= 1000000;
	}
	else if (ops_per_time >= 1000) {
		ops_unit = 'k';
		ops_per_time /= 1000;
	}
	printf("Instructions executed: %" PRIi64 " in %f ms (%f %cops/s)\n", cpu_instruction_counter, time_taken, ops_per_time, ops_unit);

	printf("\nRegisters:\nR1=0x%08x R2=0x%08x R3=0x%08x R4=0x%08x R5=0x%08x R6=0x%08x\nCSP=0x%08x BSP=0x%08x PC=0x%08x IHBASE=0x%08x\nFLAG=0b" BYTE_TO_BINARY_PATTERN " ENCREG=0x%016" PRIx64 "\n", r.r1, r.r2, r.r3, r.r4, r.r5, r.r6, r.csp, r.bsp, r.pc, r.ihbase, BYTE_TO_BINARY(r.flagr), r.encreg12);

	printf("Memory around PC(-1) region:");
	int j = 0;
	int i = opc - HEXDUMP_SIZE * 8;
	int imax = opc + HEXDUMP_SIZE * 8;
	if (i < 0) {
		i = 0;
	}
	if (imax > RAM_SIZE) {
		imax = RAM_SIZE;
	}
	uint32_t oopc = opc - 1;
	if (opc == 0) {
		oopc = RAM_SIZE - 1;
	}
	for (; i < imax; i += HEXDUMP_SIZE) {
		if (i == opc) {
			printf("\n%08x |>", i);
		} else {
			printf("\n%08x | ", i);
		}
		for (j = i; j < imax && j < i + HEXDUMP_SIZE; j++) {
			if (j == opc) {
				printf("%02x<", m[j]);
			} else if (j == oopc && j < i + (HEXDUMP_SIZE - 1)) {
				printf("%02x>", m[j]);
			} else {
				printf("%02x ", m[j]);
			}
		}
		for (; j < i + 12; j++) {
			printf("   ");
		}
		printf("| ");
		for (j = i; j < imax && j < i + 12; j++) {
			char c = m[j];
			if (c < 20 || c > 176) {
				c = '.';
			}
			putc(c, stdout);
		}
	}

	return 0;
}
