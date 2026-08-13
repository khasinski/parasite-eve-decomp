#include "common.h"

extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegResponse;
extern volatile u32 *g_CdRegRequest;
extern volatile u32 *D_8009B2B0;
extern volatile u32 *D_8009B2B4;
extern volatile u32 *D_8009B2B8;
extern volatile u32 *D_8009B2BC;
extern volatile u32 *g_CdRegDmaControl;

extern int D_8009B260;

typedef struct CdSectorDataWindow {
    int status;
    u8 pad_B264[0x4D9C];
} CdSectorDataWindow;

register CdSectorDataWindow *g_CdSectorDataWindow asm("$1");

int CD_getsector(u32 arg0, u32 arg1) {
    volatile u32 readback;
    volatile u8 *status;

    *g_CdRegIndexBase = 0;
    *g_CdRegResponse = 0x80;
    *D_8009B2B0 = 0x21020843;
    *g_CdRegRequest = 0x1325;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = arg0;
    *D_8009B2BC = arg1 | 0x10000;

    status = g_CdRegIndexBase;
    if ((*status & 0x40) == 0) {
        do {
        } while ((*status & 0x40) == 0);
    }

    *g_CdRegDmaControl = 0x11400100;
    readback = *g_CdRegDmaControl;

    goto done;

done:
    return 0;
}

void func_8007C130(int arg0) {
    g_CdSectorDataWindow = (CdSectorDataWindow *)0x800B0000;
    g_CdSectorDataWindow[-1].status = arg0;
}
