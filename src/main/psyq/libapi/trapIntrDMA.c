/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_api_internal.h"

int printf(const char *format, ...);

#define DMA_STATUS (*(volatile u32 *)g_IntrDmaDispatchPtr)

void trapIntrDMA(void) {
    u32 mask;
    int i;
    mask = (DMA_STATUS >> 24) & 0x7F;
    if (mask != 0) {
        u32 bit = 1;
        u32 preserve = 0xFFFFFF;
        DmaInterruptCallback *base;
        /* Keep loop constants initialized before the callback-table base. */
        asm volatile("" : "+r"(bit), "+r"(preserve));
        base = g_IntrDmaHandlerTable;
        do {
            for (i = 0; mask != 0 && i < 7; i++, mask >>= 1) {
                if (mask & 1) {
                    DMA_STATUS &= (bit << (i + 24)) | preserve;
                    if (base[i] != 0) {
                        base[i]();
                    }
                }
            }
            mask = (DMA_STATUS >> 24) & 0x7F;
        } while (mask != 0);
    }
    if ((DMA_STATUS & 0xFF000000) == 0x80000000 || (DMA_STATUS & 0x8000)) {
        printf(D_8001177C, DMA_STATUS);
        for (i = 0; i < 7; i++) {
            printf(D_80011798, i, D_800956E0[i].address);
        }
    }
}
