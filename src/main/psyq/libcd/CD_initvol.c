#include "common.h"
/* CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2 */

extern u8 *g_CdRegIndexBase;
extern u8 *g_CdRegPort1;
extern u8 *g_CdRegDataWrite;
extern u8 *g_CdRegResponse;
extern u16 *volatile D_8009B290;

typedef struct CdInitVolFrame {
    u8 packet[4];
    u32 pad04;
} CdInitVolFrame;

register CdInitVolFrame *g_CdInitVolFrame asm("$29");
register int g_CdInitVolValue asm("$2");
register void *g_CdInitVolIo asm("$3");

int CD_initvol(void) {
    g_CdInitVolIo = D_8009B290;
    g_CdInitVolValue = ((u16 *)g_CdInitVolIo)[0x1B8 / 2];
    g_CdInitVolFrame--;
    if (g_CdInitVolValue != 0) {
        goto set_default;
    }
    g_CdInitVolValue = ((u16 *)g_CdInitVolIo)[0x1BA / 2];
    if (g_CdInitVolValue != 0) {
        g_CdInitVolValue = 0x3FFF;
        goto store_common;
    }
    g_CdInitVolValue = 0x3FFF;
    ((u16 *)g_CdInitVolIo)[0x180 / 2] = g_CdInitVolValue;
    ((u16 *)g_CdInitVolIo)[0x182 / 2] = g_CdInitVolValue;
    g_CdInitVolIo = D_8009B290;

set_default:
    g_CdInitVolValue = 0x3FFF;

store_common:
    ((u16 *)g_CdInitVolIo)[0x1B0 / 2] = g_CdInitVolValue;
    ((u16 *)g_CdInitVolIo)[0x1B2 / 2] = g_CdInitVolValue;
    g_CdInitVolValue = 0xC001;
    ((u16 *)g_CdInitVolIo)[0x1AA / 2] = g_CdInitVolValue;

    g_CdInitVolIo = g_CdRegIndexBase;
    g_CdInitVolValue = 0x80;
    g_CdInitVolFrame->packet[2] = g_CdInitVolValue;
    g_CdInitVolFrame->packet[0] = g_CdInitVolValue;
    g_CdInitVolValue = 2;
    g_CdInitVolFrame->packet[3] = 0;
    g_CdInitVolFrame->packet[1] = 0;
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;

    g_CdInitVolIo = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[0];
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegResponse;
    g_CdInitVolValue = g_CdInitVolFrame->packet[1];
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegIndexBase;
    g_CdInitVolValue = 3;
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegPort1;
    g_CdInitVolValue = g_CdInitVolFrame->packet[2];
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[3];
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegResponse;
    g_CdInitVolValue = 0x20;
    *(u8 *)g_CdInitVolIo = g_CdInitVolValue;

    g_CdInitVolValue = 0;
    g_CdInitVolFrame++;
    return g_CdInitVolValue;
}
