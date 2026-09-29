/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern void (*volatile g_StrDataReadyCallback)(void);
void StSetStream(u32 mode, u32 startFrame, u32 endFrame,
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
