/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "common.h"
#include "pe1/gpu_callbacks.h"

extern u8 g_GraphDebug;
extern void (*g_GpuDebugPrintf[])(char *message, void *ot, int count);
extern char D_800118F8[];
extern u32 D_800957F8;
extern u32 D_8009580C;

u32 *ClearOTag(u32 *ot, int count) {
    u32 mask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (g_GraphDebug >= 2) {
        g_GpuDebugPrintf[0](D_800118F8, ot, count);
    }

    if (--count) {
        register u32 linkMask asm("$5") = 0xFFFFFF;
        u32 sizeMask = 0xFF000000;
        do {
            --count;
            ((u8 *)ot)[3] = 0;
            *ot = (*ot & sizeMask) | ((u32)(ot + 1) & linkMask);
            ++ot;
        } while (count);
    }

    mask = 0xFFFFFF;
    result = ot;
    PE1_COMPILER_LAUNDER(result);
    terminator = &D_8009580C;
    address = (u32)&D_800957F8;
    address &= mask;
    command = 0x04000000;
    address |= command;
    *terminator = address;
    terminator = (u32 *)((u32)terminator & mask);
    *result = (u32)terminator;
    return result;
}
