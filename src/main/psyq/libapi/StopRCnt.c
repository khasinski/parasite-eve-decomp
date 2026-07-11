/* MASPSX_FLAGS: --store-return-delay */

#include "pe1/psyq_api_internal.h"

int StopRCnt(unsigned int counter) {
    register int offset asm("$4");
    register int mask asm("$2");
    register volatile s32 *status asm("$5");
    register int value asm("$3");

    offset = (counter & 0xFFFF) << 2;
    status = _interrupt_status_register;
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
    status[1] = value;
    return 1;
}
