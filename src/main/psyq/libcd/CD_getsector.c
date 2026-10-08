/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBCD BIOS.OBJ: CD_getsector, CD_getsector2. */

#include "pe1/psyq_cd.h"

int CD_getsector(void *destination, int words) {
    volatile u8 *status;
    u32 ready;

    *D_8009B27C = 0;
    *D_8009B288 = 0x80;
    *D_8009B2B0 = 0x20943;
    *D_8009B28C = 0x1323;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = destination;
    *D_8009B2BC = words | 0x10000;

    status = D_8009B27C;
    do {
        ready = *status & 0x40;
    } while (!ready);

    *D_8009B2C0 = 0x11000000;
    {
        volatile u32 *control = D_8009B2C0;
        if (*control & 0x1000000) {
            volatile u32 *pending = control;
            u32 mask = 0x1000000;
            do {
            } while (*pending & mask);
        }
    }

    {
        volatile u32 *request = D_8009B28C;

        *request = 0x1325;
    }
    return 0;
}

int CD_getsector2(void *destination, int words) {
    volatile u32 readback;
    volatile u8 *index;

    *g_CdRegIndexBase = 0;
    *g_CdRegResponse = 0x80;
    *D_8009B2B0 = 0x21020843;
    *(volatile u32 *)g_CdRegRequest = 0x1325;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = destination;
    *D_8009B2BC = words | 0x10000;
    index = g_CdRegIndexBase;
    if (!(*index & 0x40)) {
        do {
        } while (!(*index & 0x40));
    }
    *g_CdRegDmaControl = 0x11400100;
    readback = *g_CdRegDmaControl;
    return 0;
}
