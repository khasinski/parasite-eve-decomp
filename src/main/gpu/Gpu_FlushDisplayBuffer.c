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

/* Read VRAM with a CPU prefix followed by 16-word DMA blocks. */
int Gpu_FlushDisplayBuffer(RECT *rect, unsigned int *data)
{
    int words, remainder, blocks;
    register unsigned int status asm("$2");
    unsigned int firstMask;
    int firstEnd;
    unsigned short width, height;
    Gpu_ResetDmaWaitTimer();
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
    asm volatile("" : "+r"(rect));
    blocks = words / 16;
    status = *D_80095854;
    /* The initial test and retry loop materialize their masks separately. */
    asm volatile("" : "+r"(status));
    firstMask = 0x04000000;
    asm volatile("" : "+r"(firstMask) : "r"(status));
    status &= firstMask;
    if (!status) {
        do {
            if (Gpu_DmaTimeoutCheck()) return -1;
        } while (!(*D_80095854 & 0x04000000));
    }
    *D_80095854 = 0x04000000;
    *D_80095850 = 0x01000000;
    *D_80095850 = 0xC0000000;
    *D_80095850 = *(unsigned int *)&rect->x;
    *D_80095850 = *(unsigned int *)&rect->w;
    status = *D_80095854;
    asm volatile("" : "+r"(status));
    firstMask = 0x08000000;
    asm volatile("" : "+r"(firstMask) : "r"(status));
    status &= firstMask;
    if (!status) {
        do {
            if (Gpu_DmaTimeoutCheck()) return -1;
        } while (!(*D_80095854 & 0x08000000));
    }
    --remainder;
    firstEnd = -1;
    /* Keep the retry loop's sentinel independent of the initial test. */
    asm volatile("" : "+r"(firstEnd) : "r"(remainder));
    if (remainder != firstEnd) {
        do {
            *data++ = *D_80095850;
        } while (--remainder != -1);
    }
    if (blocks) {
        *D_80095854 = 0x04000003;
        *D_80095858 = (unsigned int)data;
        *D_8009585C = (blocks << 16) | 16;
        *D_80095860 = 0x01000200;
    }
    return 0;
}
