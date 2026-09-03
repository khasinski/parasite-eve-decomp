/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern int (*g_MemCardStateDispatchFn)(int channel);
extern int g_MemCardDispatchResult;

int MemCard_ReadByte(int channel, int index);

int MemCard_ReadByteWithCallbackValue(int channel) {
    g_MemCardDispatchResult = g_MemCardStateDispatchFn(channel);
    return MemCard_ReadByte(channel, -2);
}
