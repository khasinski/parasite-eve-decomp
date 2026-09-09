/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

extern volatile u32 *D_8009B2B0, *D_8009B2B4, *D_8009B2B8, *D_8009B2BC;
extern volatile u32 *g_CdRegDmaControl;

int CD_getsector(u32 destination, u32 words) {
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
