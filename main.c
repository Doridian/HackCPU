#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "registers.h"
#include "io.h"

static uint8_t* fhrom_data;
static uint8_t fhrom_read(struct iostream_t* io) {
    return fhrom_data[io->rptr];
}

int main(int argc, const char **argv)
{
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
    cpu_reset();
    cpu_run();
    printf("Registers: R1=%d R2=%d R3=%d R4=%d PSP=%d CSP=%d PC=%d IHBASE=%d E=%08x\n", r.r1, r.r2, r.r3, r.r4, r.psp, r.csp, r.pc, r.ihbase, r.encreg12);
    return 0;
}
