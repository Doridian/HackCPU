#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include "registers.h"
#include "io.h"
#include "memory.h"

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
        FILE *romfh = fopen(romfname, "rb");
        fseek(romfh, 0L, SEEK_END);
        size_t romlen = ftell(romfh);
        io[IO_ROM].length = romlen;
        io[IO_ROM].read = fhrom_read;
        rewind(romfh);
        fhrom_data = malloc(romlen);
        fread(fhrom_data, romlen, 1, romfh);
        fclose(romfh);
    }
    io[IO_STDOUT].write = stdout_write;
    io[IO_STDIN].read = stdin_read;

    cpu_reset();

    uint8_t res = cpu_run();
    //if (res == ERR_HALT) {
    //    return 0;
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
		if (r.psp > 2 && r.psp <= RAM_SIZE) {
			intnum = m[r.psp - 2];
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

    printf("Registers:\nR1=%08x R2=%08x R3=%08x R4=%08x R5=%08x R6=%08x\nPSP=%08x CSP=%08x PC=%08x IHBASE=%08x ENCREG=%016" PRIx64 "\n", r.r1, r.r2, r.r3, r.r4, r.r5, r.r6, r.psp, r.csp, r.pc, r.ihbase, r.encreg12);

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
