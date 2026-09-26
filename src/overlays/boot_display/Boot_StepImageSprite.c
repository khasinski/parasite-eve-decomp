/* MASPSX_FLAGS: --expand-div */
#include "pe1/boot_display.h"
extern void *D_80125BA4[];
extern volatile s32 D_80172CD8;
extern u32 g_BootDisplayFrameCounter;
extern char *g_BootDisplayTransferCursor;
extern u32 func_8010BF94(void *);
extern void func_8010C86C(u32);
extern s32 func_8010C89C(void *, void *, void *);
extern void func_8010BFA0(void *, s32);
extern void func_8010C01C(void *, s32);
extern s32 func_8010C078(s32);
void Boot_StepImageSprite(void) {
    BootDisplaySpritePacket *packet = (BootDisplaySpritePacket *)g_BootDisplayPrimitiveCursor;
    u16 tpage;
    s16 rect[4];
    tpage = func_80077A64(2, 0, g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].x, g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].y);
    func_80077C84(packet, 0, 0, tpage);
    /* PsyQ packet length occupies the high byte of its ordering-table tag. */
    ((u8 *)&packet->sprite.tag)[3] = 4;
    packet->sprite.code = 0x64;
    func_80077CB4(packet, &packet->sprite);
    switch ((u16)g_BootDisplayStateTimer & 3) {
    case 0: packet->sprite.x0 = 160; packet->sprite.y0 = 0; break;
    case 1: packet->sprite.x0 = 0; packet->sprite.y0 = 0; break;
    case 2: packet->sprite.x0 = 160; packet->sprite.y0 = 105; break;
    case 3: packet->sprite.x0 = 0; packet->sprite.y0 = 105; break;
    }
    packet->sprite.u0 = 0;
    packet->sprite.v0 = 0;
    packet->sprite.r0 = D_80172CD8;
    packet->sprite.g0 = D_80172CD8;
    packet->sprite.b0 = D_80172CD8;
    packet->sprite.w = 160;
    packet->sprite.h = 120;
    if (g_BootDisplayTransition == 0) {
        s32 brightness = D_80172CD8 + 4;
        D_80172CD8 = brightness;
        if (brightness >= 128) {
            D_80172CD8 = 128;
            g_BootDisplayTransition = 1;
        }
    } else if (g_BootDisplayTransition == 2) {
        s32 brightness = D_80172CD8 - 4;
        D_80172CD8 = brightness;
        if (brightness <= 0) {
            D_80172CD8 = 0;
            g_BootDisplayTransition = 1;
        }
    }
    func_80077AC4(g_BootDisplayOrderingTable + 56, packet);
    g_BootDisplayPrimitiveCursor += sizeof(*packet);
    switch ((u32)g_BootDisplayState) {
    case 0: {
        s32 offset;
        s32 startOffset;
        s32 endOffset;
        char *base;

        func_8010C86C(func_8010BF94(D_80125BA4[(u16)g_BootDisplayStateTimer]) >> 2);
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
        offset = (D_80093168 - D_80093166) * 2048;
        g_BootDisplayTransferCursor = D_80011610 + offset;
        func_8010BFA0(D_80011610 + (offset + 0x38800), 0);
        func_8010C01C(g_BootDisplayTransferCursor, 0x2800);
        g_BootDisplayState = 3;
        break;
    }
    case 3:
        if (func_8010C078(1) == 0) {
            g_BootDisplayState = 4;
            g_BootDisplayTransferProgress = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX;
        }
        break;
    case 4:
        if ((u16)g_BootDisplayTransferProgress < g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX + 160) {
            rect[0] = g_BootDisplayTransferProgress;
            rect[1] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferY;
            rect[2] = 16;
            rect[3] = 128;
            func_8007506C(rect, g_BootDisplayTransferCursor);
            g_BootDisplayTransferCursor += 0x1000;
            g_BootDisplayTransferProgress += 16;
        } else g_BootDisplayState = 5;
        break;
    case 5: {
        register u32 frame asm("$2") = g_BootDisplayFrameCounter;
        u32 divisor = 450;
        u32 phase;
        register u32 threshold asm("$2");
        /* Preserve retail hardware division rather than reciprocal multiplication. */
        asm("" : "=r"(divisor) : "0"(divisor));
        phase = frame % divisor;
        threshold = divisor - 93;
        if (phase == threshold) g_BootDisplayTransition = 2;
        if (phase == 0) {
            asm("" : "=r"(phase) : "0"(phase));
            g_BootDisplayState = 6;
            g_BootDisplayTransition = 0;
        }
        break;
    }
    case 6: {
        u16 timer = (u16)g_BootDisplayStateTimer + 1;
        g_BootDisplayStateTimer = timer;
        if (timer < 30) {
            g_BootDisplayState = 0;
        } else {
            g_BootDisplayState = 7;
            asm("" : : : "memory");
        }
        g_BootDisplayTransition = 0;
        g_BootDisplayBufferIndex ^= 1;
        break;
    }
    case 7: {
        register u32 frame asm("$2") = g_BootDisplayFrameCounter;
        u32 divisor = 450;
        u32 phase;
        register u32 threshold asm("$2");
        /* Preserve retail hardware division rather than reciprocal multiplication. */
        asm("" : "=r"(divisor) : "0"(divisor));
        phase = frame % divisor;
        threshold = divisor - 93;
        if (phase == threshold) g_BootDisplayTransition = 2;
        break;
    }
    }
}
