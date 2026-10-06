/* ASSEMBLER: GNU */
/* PSY-Q LIBAPI PAD, part 1 of 6: SetInitPadFlag, ReadInitPadFlag. */
#include "pe1/pad_internal.h"

void SetInitPadFlag(int value) {
    g_InitPadFlag = value;
}

int ReadInitPadFlag(void) {
    return g_InitPadFlag;
}
