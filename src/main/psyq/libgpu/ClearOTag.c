/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"

extern char D_800118F8[];
extern u32 D_800957F8;
extern u32 D_8009580C;

u32 *ClearOTag(u32 *ot, int count) {
    u32 mask;
    u32 linkMask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_800118F8, ot, count);
    }

    if (--count) {
        do {
            --count;
            linkMask = 0xFFFFFF;
            ((u8 *)ot)[3] = 0;
            mask = 0xFF000000;
            *ot = (*ot & mask) | ((u32)(ot + 1) & linkMask);
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
