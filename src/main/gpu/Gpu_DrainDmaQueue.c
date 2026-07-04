/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Gpu_SetDisplayBuffer(void);
void Gpu_ResetDmaWaitTimer(void);
s32 Gpu_DmaTimeoutCheck(void);
extern s32 * g_GpuGp1Ptr[];
#define g_GpuGp1Ptr (g_GpuGp1Ptr[0])
extern s32 * g_GpuDmaChcrPtr[];
#define g_GpuDmaChcrPtr (g_GpuDmaChcrPtr[0])
extern s32 g_GpuDmaQueueHead[];
#define g_GpuDmaQueueHead (g_GpuDmaQueueHead[0])
extern s32 g_GpuDmaQueueTail[];
#define g_GpuDmaQueueTail (g_GpuDmaQueueTail[0])

s32 Gpu_DrainDmaQueue(s32 arg0) {
    s32 temp_s0;
    s32 var_v0;

    if (arg0 == 0) {
        Gpu_ResetDmaWaitTimer();
        goto loop_3_check;
loop_3_body:
        Gpu_SetDisplayBuffer();
        if (Gpu_DmaTimeoutCheck() != 0) {
            return -1;
        }
loop_3_check:
        if (g_GpuDmaQueueHead != g_GpuDmaQueueTail) {
            goto loop_3_body;
        }
        goto loop_6_check;
loop_6_body:
        if (Gpu_DmaTimeoutCheck() != 0) {
            return -1;
        }
loop_6_check:
        if (*g_GpuDmaChcrPtr & 0x01000000) {
            goto loop_6_body;
        }
        __asm__ volatile("" ::: "memory");
        if (*g_GpuGp1Ptr & 0x04000000) {
            return 0;
        }
        goto loop_6_body;
    }
    temp_s0 = (g_GpuDmaQueueHead - g_GpuDmaQueueTail) & 0x3F;
    if (temp_s0 != 0) {
        Gpu_SetDisplayBuffer();
    }
    if (*g_GpuDmaChcrPtr & 0x01000000) {
        goto block_17;
    }
    __asm__ volatile("" ::: "memory");
    if (*g_GpuGp1Ptr & 0x04000000) {
        goto block_19;
    }
block_17:
    if (temp_s0 != 0) {
        goto block_19;
    }
    return 1;
block_19:
    return temp_s0;
}
