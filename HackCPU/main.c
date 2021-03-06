#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include "registers.h"
#include "io.h"
#include "memory.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(val)  \
  (val & 0x80 ? '1' : '0'), \
  (val & 0x40 ? '1' : '0'), \
  (val & 0x20 ? '1' : '0'), \
  (val & 0x10 ? '1' : '0'), \
  (val & 0x08 ? '1' : '0'), \
  (val & 0x04 ? '1' : '0'), \
  (val & 0x02 ? '1' : '0'), \
  (val & 0x01 ? '1' : '0') 

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

static void stdout_write(uint32_t id, iostream_t* io, uint8_t i) {
	putc(i, stdout);
}

static uint8_t stdin_read(uint32_t id, iostream_t* io) {
	return getc(stdin);
}

static uint8_t* fhrom_data;
static uint8_t fhrom_read(uint32_t id, iostream_t* io) {
	return fhrom_data[io->rptr];
}

#define HEXDUMP_SIZE 12

static int strtol_unit(const char *arg) {
	size_t len = strlen(arg);
	int multiplier = 1;
	if (len > 1) {
		int striplast = 1;
		switch (arg[len - 1]) {
		case 'k':
			multiplier = 1024;
			break;
		case 'M':
			multiplier = 1024 * 1024;
			break;
		case 'G':
			multiplier = 1024 * 1024 * 1024;
			break;
		default:
			striplast = 0;
			break;
		}
	}
	return strtol(arg, NULL, 10) * multiplier;
}

int main(int argc, const char **argv) {
	const char *romfname = NULL;
	uint32_t ramsize = RAM_SIZE_DEFAULT;

	char opt = 0;
	for (int i = 1; i < argc; i++) {
		const char* arg = argv[i];
		if (opt != 0) {
			switch (opt) {
			case 'm':
				ramsize = strtol_unit(arg);
				break;
			default:
				printf("Option %c unknown\n", opt);
				return 1;
			}
			opt = 0;
		} else if (arg[0] == '-') {
			if (strlen(arg) != 2) {
				printf("Options need a single char name, a dangling - or -longname is invalid\n");
				return 1;
			}
			opt = arg[1];
		}
		else {
			romfname = arg;
		}
	}

	if (opt != 0) {
		printf("Options need values\n");
		return 1;
	}

	cpu_state s = cpu_init(IO_COUNT_DEFAULT, 1, ramsize);

	if (romfname != NULL) {
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
		s->io[IO_ROM]->length = (uint32_t)romlen;
		s->io[IO_ROM]->read = fhrom_read;
		rewind(romfh);
		fhrom_data = (uint8_t*)malloc(romlen);
		fread(fhrom_data, romlen, 1, romfh);
		fclose(romfh);
	}

	s->io[IO_STDOUT]->write = stdout_write;
	s->io[IO_STDIN]->read = stdin_read;

	cpu_reset(s);


	uint64_t start_time = get_highres_timestamp();
	uint8_t res = cpu_run(s);
	uint64_t end_time = get_highres_timestamp();
	//if (res == ERR_HALT) {
	//	return 0;
	//}
	printf("\n\n\nCPU exit code: %02x", res);

	uint32_t opc = s->reg.pc - 1;
	if (s->reg.pc == 0) {
		opc = s->ram_size - 1;
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
		if (s->reg.csp > 2 && s->reg.csp <= s->ram_size) {
			intnum = s->ram[s->reg.csp - 2];
		} else {
			intnum = 0xFF;
		}
		printf(" (Unhandled interrupt %02x", intnum);
		switch (intnum) {
		case INT_ERR:
			printf(" (CPU Error)");
			break;
		case INT_ILLEGAL_OPCODE:
			printf(" (Illegal opcode %02x)", s->ram[opc]);
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
	double ops_per_time = (s->instruction_counter / time_taken) * 1000.0;
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
	printf("Instructions executed: %" PRIi64 " in %f ms (%f %cops/s)\n", s->instruction_counter, time_taken, ops_per_time, ops_unit);

	printf("\nRegisters:\nR1=0x%08x R2=0x%08x R3=0x%08x R4=0x%08x R5=0x%08x R6=0x%08x\nCSP=0x%08x BSP=0x%08x PC=0x%08x IHBASE=0x%08x\nFLAG=0b" BYTE_TO_BINARY_PATTERN " ENCREG=0x%016" PRIx64 "\n", s->reg.r1, s->reg.r2, s->reg.r3, s->reg.r4, s->reg.r5, s->reg.r6, s->reg.csp, s->reg.bsp, s->reg.pc, s->reg.ihbase, BYTE_TO_BINARY(s->reg.flagr), s->reg.encreg12);

	printf("Memory around PC(-1) region:");
	uint32_t j = 0;
	uint32_t i = 0;
	uint32_t imax = opc + HEXDUMP_SIZE * 8;
	if (opc > HEXDUMP_SIZE * 8) {
		i = opc - HEXDUMP_SIZE * 8;
	}

	if (imax > s->ram_size) {
		imax = s->ram_size;
	}
	uint32_t oopc = opc - 1;
	if (opc == 0) {
		oopc = s->ram_size - 1;
	}
	for (; i < imax; i += HEXDUMP_SIZE) {
		if (i == opc) {
			printf("\n%08x |>", i);
		} else {
			printf("\n%08x | ", i);
		}
		for (j = i; j < imax && j < i + HEXDUMP_SIZE; j++) {
			if (j == opc) {
				printf("%02x<", s->ram[j]);
			} else if (j == oopc && j < i + (HEXDUMP_SIZE - 1)) {
				printf("%02x>", s->ram[j]);
			} else {
				printf("%02x ", s->ram[j]);
			}
		}
		for (; j < i + 12; j++) {
			printf("   ");
		}
		printf("| ");
		for (j = i; j < imax && j < i + 12; j++) {
			char c = s->ram[j];
			if (c < 20 || c > 176) {
				c = '.';
			}
			putc(c, stdout);
		}
	}

	return 0;
}
