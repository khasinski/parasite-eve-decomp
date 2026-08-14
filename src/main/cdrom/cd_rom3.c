
#include "pe1/psyq_cd.h"

extern u32 g_DsReadStatusBlock[];

u32 DsSync(u32 mode) {
    u32 offset = mode;
    u32 result;
    /* Match note: keep the address expansion in $v0 without a maspsx temp-dest flag. */
    asm volatile(
        "sll\t%1,%1,2\n\t"
        "lui\t%0,%%hi(g_DsReadStatusBlock)\n\t"
        "addu\t%0,%0,%1\n\t"
        "lw\t%0,%%lo(g_DsReadStatusBlock)(%0)"
        : "=r"(result), "=r"(offset)
        : "1"(offset));
    return result;
}
