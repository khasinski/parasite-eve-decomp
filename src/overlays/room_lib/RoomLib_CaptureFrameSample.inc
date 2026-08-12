#include "common.h"

typedef struct RoomCaptureContext {
    unsigned char pad00[8];
    unsigned char *volatile object;
} RoomCaptureContext;

extern RoomCaptureContext *D_800F32D0;
extern u16 g_FrameCount16;
extern s16 D_80192C34;
extern volatile s16 D_80192C36;
extern s16 D_80192C38;
extern s32 D_80192C3C;

void func_80192A3C(RoomCaptureContext *modeOrContext, int x, int y, int z) {
    unsigned int sample;
    unsigned char *object;
    volatile s16 *captureX;

    if ((int)modeOrContext == 1) {
        D_80192C34 = x;
        D_80192C36 = y;
        D_80192C38 = z;
    } else {
        D_80192C3C = x;
    }

    modeOrContext = D_800F32D0;
    captureX = &D_80192C34;
    object = modeOrContext->object;
    sample = *(u16 *)(object + 0x268);
    *captureX = sample;
    sample = (unsigned int)modeOrContext->object;
    sample = *(u16 *)(sample + 0x26a);
    D_80192C36 = sample;
    sample = (unsigned int)modeOrContext->object;
    modeOrContext = (RoomCaptureContext *)(unsigned int)g_FrameCount16;
    x = *(u16 *)(sample + 0x26c);
    sample = 0x76c;
    D_80192C36 = (unsigned int)modeOrContext;
    D_80192C3C = sample;
    D_80192C38 = x;
}
