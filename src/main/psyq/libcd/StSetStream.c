/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern volatile s32 D_800C0DB8;
extern void (*volatile g_StrDataReadyCallback)(void);
extern volatile s32 D_800A801C;
extern volatile s32 D_800B8620;
extern volatile s32 D_800B6914;
extern volatile s16 D_800A8018;
extern volatile s32 D_800A5D54;
extern void (*volatile D_800B0CCC)(void);

void StSetMask(s32 mask, s32 start, s32 end);

void StSetStream(s32 mode, s32 startFrame, s32 endFrame,
                 void (*callback1)(void), void (*callback2)(void)) {
    StSetMask(1, startFrame, endFrame);
    D_800C0DB8 = 0;
    g_StrDataReadyCallback = callback1;
    D_800A801C = mode & 1;
    D_800B8620 = 0;
    D_800B6914 = 0;
    D_800A8018 = 0;
    D_800A5D54 = 0;
    D_800B0CCC = callback2;
    asm volatile("" : : : "memory");
}
