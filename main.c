#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "registers.h"

int main()
{
    cpu_init();
    cpu_run();
    printf("Registers: R1=%d R2=%d R3=%d R4=%d SP=%d PC=%d IHBASE=%d\n", r.r1, r.r2, r.r3, r.r4, r.sp, r.pc, r.ihbase);
    return 0;
}
