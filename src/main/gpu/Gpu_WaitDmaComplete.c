/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
extern short D_80095750;
extern short D_80095752;
extern unsigned int *D_80095850;
extern unsigned int *D_80095854;
extern unsigned int *D_80095858;
extern unsigned int *D_8009585C;
extern unsigned int *D_80095860;
void Gpu_ResetDmaWaitTimer(void);
int Gpu_DmaTimeoutCheck(void);

/* Upload pixels using a CPU prefix followed by 16-word DMA blocks. */
int Gpu_WaitDmaComplete(RECT *rect, unsigned int *data)
{
    int words, remainder, blocks;
    register int variant asm("$21");
    unsigned int status;
    register unsigned int firstMask asm("$3");
    int firstEnd;
    unsigned int command;
    unsigned int *port;
    unsigned short width, height;
    Gpu_ResetDmaWaitTimer();
    variant = 0;
    if (rect->w >= 0) {
        if (D_80095750 < rect->w) width = D_80095750;
        else width = rect->w;
    } else width = 0;
    rect->w = width;
    if (rect->h >= 0) {
        if (D_80095752 < rect->h) height = D_80095752;
        else height = rect->h;
    } else height = 0;
    rect->h = height;
    words = (rect->w * rect->h + 1) / 2;
    if (words <= 0) return -1;
    remainder = words % 16;
    /* Retail retains the variant branch and this rectangle's register lifetime. */
    asm volatile("" : "+r"(variant), "+r"(rect));
    blocks = words / 16;
    status = *D_80095854;
    /* Keep the first readiness mask separate from the loop's invariant. */
    asm volatile("" : "+r"(status));
    firstMask = 0x04000000;
    asm volatile("" : "+r"(firstMask) : "r"(status));
    if (!(status & firstMask)) {
        do {
            if (Gpu_DmaTimeoutCheck()) return -1;
        } while (!(*D_80095854 & 0x04000000));
    }
    command = 0xA0000000;
    asm volatile("" : "+r"(command));
    *D_80095854 = 0x04000000;
    *D_80095850 = 0x01000000;
    port = D_80095850;
    if (variant) command = 0xB0000000;
    *port = command;
    *D_80095850 = *(unsigned int *)&rect->x;
    *D_80095850 = *(unsigned int *)&rect->w;
    --remainder;
    firstEnd = -1;
    /* The loop materializes its own sentinel after this first test. */
    asm volatile("" : "+r"(firstEnd) : "r"(remainder));
    if (remainder != firstEnd) {
        do {
            *D_80095850 = *data++;
        } while (--remainder != -1);
    }
    if (blocks) {
        *D_80095854 = 0x04000002;
        *D_80095858 = (unsigned int)data;
        *D_8009585C = (blocks << 16) | 16;
        *D_80095860 = 0x01000201;
    }
    return 0;
}
