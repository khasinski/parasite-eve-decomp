/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
#include "common.h"

extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegPort1;
extern volatile u8 *g_CdRegDataWrite;
extern volatile u8 *g_CdRegResponse;
extern volatile u16 *D_8009B290;

int CD_initvol(void) {
    volatile u8 vol[4];

    if (D_8009B290[0x1B8 / 2] == 0 && D_8009B290[0x1BA / 2] == 0) {
        D_8009B290[0x180 / 2] = 0x3FFF;
        D_8009B290[0x182 / 2] = 0x3FFF;
    }

    D_8009B290[0x1B0 / 2] = 0x3FFF;
    D_8009B290[0x1B2 / 2] = 0x3FFF;
    D_8009B290[0x1AA / 2] = 0xC001;

    vol[2] = 0x80;
    vol[0] = 0x80;
    vol[3] = 0;
    vol[1] = 0;

    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = vol[0];
    *g_CdRegResponse = vol[1];
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = vol[2];
    *g_CdRegDataWrite = vol[3];
    *g_CdRegResponse = 0x20;

    return 0;
}
