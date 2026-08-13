
#include "pe1/psyq_api_internal.h"

int StopRCnt(unsigned int counter) {
    int offset;
    int mask;
    register InterruptControlRegisters *status asm("$5");
    int value;
    offset = (counter & 0xFFFF) << 2;
    status = D_8009B7CC;
    /* Match the original %hi/%lo indexed mask load without using $at. */
    asm volatile(
        ".set\tnoreorder\n\t"
        "lui\t$2,%%hi(D_8009B7D4)\n\t"
        "addu\t$2,$2,$4\n\t"
        "lw\t$2,%%lo(D_8009B7D4)($2)\n\t"
        "lw\t$3,4($5)\n\t"
        "nor\t$2,$zero,$2\n\t"
        "and\t$3,$3,$2\n\t"
        ".set\treorder"
        : "=r"(mask), "=r"(value)
        : "r"(offset), "r"(status));
    status->mask = value;
    return 1;
}
