/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_api_internal.h"

typedef struct DmaChannelRegisters {
    u32 address;
    u32 block_control;
    u32 channel_control;
    u32 unused;
} DmaChannelRegisters;
PE1_STATIC_ASSERT(sizeof(DmaChannelRegisters) == 16, dma_channel_stride);
extern DmaChannelRegisters *D_800956E0;
extern char D_8001177C[];
extern char D_80011798[];
int printf(const char *format, ...);

#define DMA_STATUS (*(volatile u32 *)g_IntrDmaDispatchPtr)

void trapIntrDMA(void) {
    u32 mask;
    int i;
    while ((mask = (DMA_STATUS >> 24) & 0x7F) != 0) {
        DmaInterruptCallback *entry = g_IntrDmaHandlerTable;
        for (i = 0; mask != 0 && i < 7; i++, mask >>= 1, entry++) {
            if (mask & 1) {
                DMA_STATUS &= 0xFFFFFF | (1U << (i + 24));
                if (*entry != 0) {
                    (*entry)();
                }
            }
        }
    }
    if ((DMA_STATUS & 0xFF000000) == 0x80000000 || (DMA_STATUS & 0x8000)) {
        printf(D_8001177C, DMA_STATUS);
        for (i = 0; i < 7; i++) {
            printf(D_80011798, i, D_800956E0[i].address);
        }
    }
}
