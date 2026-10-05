#ifndef PE1_GPU_DMA_REGISTER_WINDOW_H
#define PE1_GPU_DMA_REGISTER_WINDOW_H

#include "common.h"

/*
 * Register-pointer window beginning at the GP1 pointer word (0x80095854).
 * This overlapping view keeps the original absolute symbol at offset zero,
 * while naming the adjacent DMA address/count/control pointers.
 */
typedef struct GpuDmaRegisterWindow {
    /* 0x00 */ u32 *gp1;
    /* 0x04 */ u32 *madr;
    /* 0x08 */ u32 *bcr;
    /* 0x0C */ u32 *chcr;
} GpuDmaRegisterWindow;

#if defined(__mips__)
PE1_STATIC_ASSERT(sizeof(GpuDmaRegisterWindow) == 0x10,
                  gpu_dma_register_window_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GpuDmaRegisterWindow, madr) == 0x04,
                  gpu_dma_register_window_madr_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GpuDmaRegisterWindow, bcr) == 0x08,
                  gpu_dma_register_window_bcr_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GpuDmaRegisterWindow, chcr) == 0x0C,
                  gpu_dma_register_window_chcr_offset);
#endif

#endif /* PE1_GPU_DMA_REGISTER_WINDOW_H */
