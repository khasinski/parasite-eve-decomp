#include "common.h"

/* MASPSX_FLAGS: --store-return-delay */

extern int g_TimerTimeoutLimit;
extern int D_800A76D0;

void Timer_StartTimeout(int limit) {
    volatile u16 *timer = (volatile u16 *)0x1F801120;
    int start;

    start = *timer;
    g_TimerTimeoutLimit = limit;
    D_800A76D0 = start;
}
