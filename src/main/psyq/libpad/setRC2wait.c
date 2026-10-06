/* ASSEMBLER: GNU */
#include "pe1/psyq_pad_main.h"

extern int g_TimerTimeoutLimit;

void setRC2wait(int limit) {
    int start = *(volatile u16 *)0x1F801120;
    g_TimerTimeoutLimit = limit;
    D_800A76D0 = start;
}
