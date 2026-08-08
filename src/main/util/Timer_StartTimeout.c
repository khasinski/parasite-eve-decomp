#include "common.h"
/* MASPSX_FLAGS: --store-return-delay */

extern int g_TimerTimeoutLimit;
extern int g_TimerTimeoutStart;

void Timer_StartTimeout(int arg0) {
    volatile u16 *timer = (volatile u16 *)0x1F801120;

    g_TimerTimeoutLimit = arg0;
    g_TimerTimeoutStart = *timer;
}
