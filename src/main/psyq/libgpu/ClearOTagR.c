/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"

extern char D_80011910[];
extern u32 D_800957F8;
extern u32 D_8009580C;

u32 *ClearOTagR(u32 *ot, int count) {
    u32 mask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011910, ot, count);
    }

    D_80095744->clearOTag(ot, count);

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
