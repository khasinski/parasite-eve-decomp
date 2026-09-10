/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern volatile u8 *D_8009B27C;
extern volatile u8 *D_8009B288;
extern volatile u32 *D_8009B28C;
extern volatile u32 *D_8009B2B0;
extern volatile u32 *D_8009B2B4;
extern void *volatile *D_8009B2B8;
extern volatile u32 *D_8009B2BC;
extern volatile u32 *D_8009B2C0;

int CD_getsector2(void *destination, int words) {
    volatile u8 *status;
    volatile u32 *control;

    *D_8009B27C = 0;
    *D_8009B288 = 0x80;
    *D_8009B2B0 = 0x20943;
    *D_8009B28C = 0x1323;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = destination;
    *D_8009B2BC = words | 0x10000;

    status = D_8009B27C;
    do {
    } while (!(*status & 0x40));

    *D_8009B2C0 = 0x11000000;
    control = D_8009B2C0;
    if (*control & 0x1000000) {
        volatile u32 *pending = control;
        u32 mask = 0x1000000;

        do {
        } while (*pending & mask);
    }

    *D_8009B28C = 0x1325;
    return 0;
}
