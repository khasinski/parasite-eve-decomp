/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern int D_800A3610[];
extern u8 D_800A3614[];
extern int D_800A3690;

void Util_Copy8(u8 *destination, u8 *source);

void CdRom_EnqueueCmd(int value, int command, u8 *payload) {
    register int *cursor asm("$16");
    u8 *payloadBase;

    cursor = &D_800A3690;
    asm volatile("" : "+r"(cursor));

    D_800A3610[*cursor << 2] = value;
    D_800A3614[*cursor << 4] = command;

    payloadBase = (u8 *)cursor - 123;
    Util_Copy8((u8 *)((*cursor << 4) + (int)payloadBase), payload);

    (*cursor)++;
    if (*cursor >= 8) {
        *cursor = 0;
    }
}
