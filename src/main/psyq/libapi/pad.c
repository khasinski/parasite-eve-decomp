/* ASSEMBLER: GNU */
/* PSY-Q LIBAPI PAD, part 1 of 6: SetInitPadFlag, ReadInitPadFlag. */
/* LIBAPI PAD stays in six units: Sys_SetInterruptHandler (pad_5.c) only
 * matches with GCC 2.8.1 and -mno-split-addresses, pad_4.c and pad_6.c
 * only with GCC 2.7.2, and pad_2.c and pad_3.c instantiate Pad_InitCommon.inc
 * with different names. */
#include "pe1/pad_internal.h"

void SetInitPadFlag(int value) {
    g_InitPadFlag = value;
}

int ReadInitPadFlag(void) {
    return g_InitPadFlag;
}
