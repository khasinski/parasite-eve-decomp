#include "common.h"

typedef struct TimerLimitPage {
    int limit;
    char reserved04[0x2FD0];
} TimerLimitPage;

typedef struct TimerStartPage {
    char reserved00[0x76D0];
    int start;
} TimerStartPage;

register void *g_TimerWritePage asm("$1");

void Timer_StartTimeout(int limit) {
    volatile u16 *timer = (volatile u16 *)0x1F801120;
    int start;

    start = *timer;
    g_TimerWritePage = (void *)0x800C0000;
    ((TimerLimitPage *)g_TimerWritePage)[-1].limit = limit;
    g_TimerWritePage = (void *)0x800A0000;
    ((TimerStartPage *)g_TimerWritePage)->start = start;
}
