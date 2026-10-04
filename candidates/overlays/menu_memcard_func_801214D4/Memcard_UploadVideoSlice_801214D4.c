#include "menu_memcard_video.h"

/* Uploads the next slice of the decoded frame to VRAM and flips the slice
 * buffers; when a frame is complete, switches to the other display region
 * and applies a pending colour depth change. The selector is re-read after
 * the first read (two lbu in retail), so it is accessed as volatile. */
void func_801214D4(void) {
    VideoRect rectangle;
    volatile u8 *selector;
    s8 old;
    s32 next;
    s32 region;
    s16 x;

    if (D_800B0DBB && g_CdStreamReadyHalfword) {
        func_8007C564();
        g_CdStreamReadyHalfword = 0;
    }
    rectangle = D_801228CC.rect;
    selector = &D_801228CC.selector;
    old = *selector;
    next = *selector ^ 1;
    *selector = next;
    x = D_801228CC.rect.x + D_801228CC.rect.w;
    region = D_801228CC.region;
    D_801228CC.rect.x = x;
    if (x < D_801228CC.regions[region].x + D_801228CC.regions[region].w) {
        func_8010C01C(D_801228CC.buffers[next],
                      D_801228CC.rect.w * D_801228CC.rect.h / 2);
    } else {
        D_801228CC.done = 1;
        region ^= 1;
        D_801228CC.region = region;
        D_801228CC.rect.x = D_801228CC.regions[region].x;
        D_801228CC.rect.y = D_801228CC.regions[region].y;
        if (D_801223F8 == 1) {
            D_800B0DBB ^= 1;
            if (D_800B0DBB) {
                D_801228CC.rect.w = 24;
            } else {
                D_801228CC.rect.w = 16;
            }
            func_80121004(D_8009CDDC ^ 1, D_800B0DBB);
            D_801223F8 = 2;
        }
    }
    func_8007506C(&rectangle, D_801228CC.buffers[old]);
}
