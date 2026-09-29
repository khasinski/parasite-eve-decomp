#ifndef PE1_PSYQ_LIBCD_BIOS_INTERNAL_H
#define PE1_PSYQ_LIBCD_BIOS_INTERNAL_H

#include "pe1/psyq_cd.h"
#include "pe1/psyq_bios.h"

/* Compare the previous poll count, then commit the incremented counter. */
static inline int count_poll(void) {
    int count = D_800A347C;
    int old = count++;
    D_800A347C = count;
    return old;
}

static inline int timed_out(char **commands, char **events,
                            CdInterruptEvents *interrupts) {
    if (VSync(-1) > D_800A3478 || count_poll() > 0x3C0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[g_CdLastCom],
               events[interrupts->sync], events[interrupts->ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

static inline void copy_result(u8 *destination, const volatile u8 *source) {
    int remaining;
    if (destination) {
        remaining = 7;
        for (; remaining != -1; remaining--) {
            *destination++ = *source++;
        }
    }
}

static inline void dispatch_interrupts(u8 *sync, u8 *ready) {
    int bank = *D_8009B27C & 3;
    int pending;
    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8)
            D_8009AFB8(*ready, D_800A3468);
        if ((pending & 2) && D_8009AFB4)
            D_8009AFB4(*sync, D_800A3460);
    }
    *D_8009B27C = bank;
}

#endif
