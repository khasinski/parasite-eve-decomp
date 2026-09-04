/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"

extern u8 g_GraphDebug;
extern void (*g_GpuDebugPrintf[])(char *message, void *ot, int count);
extern GpuCallbacks *g_GpuCallbacks[];
extern char D_80011910[];
extern u32 D_800957F8;
extern u32 D_8009580C;

typedef void (*GpuClearOtCallback)(u32 *ot, int count);

u32 *ClearOTagR(u32 *ot, int count) {
    u32 mask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (g_GraphDebug >= 2) {
        g_GpuDebugPrintf[0](D_80011910, ot, count);
    }

    (*(GpuClearOtCallback *)((u8 *)g_GpuCallbacks[0] + 0x2C))(ot, count);

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
