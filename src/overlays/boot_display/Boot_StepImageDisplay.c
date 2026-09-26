#include "pe1/boot_display.h"
extern void *D_80125BA4[];
extern s32 D_8009D26C, D_8009D1F4;
extern u32 g_BootDisplayFrameCounter;
extern char *g_BootDisplayTransferCursor;
extern void func_800750CC(void *, void *);
extern u32 func_8010BF94(void *);
extern void func_8010C86C(u32);
extern s32 func_8010C89C(void *, void *, void *);
extern void func_8010BFA0(void *, s32);
extern void func_8010C01C(void *, s32);
extern s32 func_8010C078(s32);
void Boot_StepImageDisplay(void) {
    s16 rect[4];
    rect[0] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].x;
    rect[1] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].y;
    rect[2] = 480;
    rect[3] = 224;
    func_800750CC(rect, g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].pixels);
    func_80074DC0(0);
    if (D_8009D26C < 0) {
        if (D_8009D1F4 & 0x1000000) {
            u16 timer = (u16)g_BootDisplayStateTimer;
            if (timer != 0) {
                g_BootDisplayStateTimer = timer - 1;
                g_BootDisplayState = 0;
            }
        } else if (D_8009D1F4 & 0x2000000) {
            u16 timer = (u16)g_BootDisplayStateTimer;
            if (timer < 30) {
                g_BootDisplayStateTimer = timer + 1;
                g_BootDisplayState = 0;
            }
        }
    }
    switch ((u32)g_BootDisplayState) {
    case 0: {
        s32 offset;
        s32 startOffset;
        s32 endOffset;
        char *base;

        func_8010C86C(func_8010BF94(D_80125BA4[(u16)g_BootDisplayStateTimer]) >> 4);
        startOffset = 0x38800;
        endOffset = 0x43000;
        offset = (D_80093168 - D_80093166) * 2048;
        base = D_80011610;
        startOffset += offset;
        func_8010C89C(D_80125BA4[(u16)g_BootDisplayStateTimer], base + startOffset, base + (offset + endOffset));
        g_BootDisplayState = 1;
        break;
    }
    case 1: {
        s32 offset;
        offset = (D_80093168 - D_80093166) * 2048;
        if (func_8010C89C(0, 0, D_80011610 + (offset + 0x43000)) == 0) {
            g_BootDisplayState = 2;
            g_BootDisplayTransferProgress = 0;
        }
        break;
    }
    case 2: {
        s32 offset;
        g_BootDisplayTransferCursor = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].decodeTarget;
        func_8010BE3C(0);
        offset = (D_80093168 - D_80093166) * 2048;
        func_8010BFA0(D_80011610 + (offset + 0x38800), 1);
        func_8010C01C(g_BootDisplayTransferCursor, 0x3C00);
        g_BootDisplayState = 3;
        break;
    }
    case 3:
        if (func_8010C078(1) == 0) {
            g_BootDisplayState = 4;
            g_BootDisplayTransferProgress = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX + 120;
        }
        break;
    case 4:
        if ((u16)g_BootDisplayTransferProgress < g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX + 360) {
            rect[0] = g_BootDisplayTransferProgress;
            rect[1] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferY + 52;
            rect[2] = 24;
            rect[3] = 120;
            func_8007506C(rect, g_BootDisplayTransferCursor);
            g_BootDisplayTransferCursor += 0x1800;
            g_BootDisplayTransferProgress += 24;
        } else g_BootDisplayState = 5;
        break;
    case 5:
        if (g_BootDisplayFrameCounter % 450 == 0) g_BootDisplayState = 6;
        break;
    case 6:
    case 7:
        break;
    }
}
