#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

int main()
{
    cpu_init();
    cpu_run();
    printf("Hello world!\n");
    return 0;
}
