/* ASSEMBLER: GNU */
#include "pe1/sys_reset.h"

s32 *func_8010BEFC(s32 *source)
{
    register s32 *read = source;
    register s32 *write = D_8010DA10;
    int index;

    for (index = 15; index != -1; index--)
        *write++ = *read++;

    write = D_8010DA50;
    read = source + 16;
    for (index = 15; index != -1; index--)
        *write++ = *read++;

    func_8010C1EC(D_8010DA0C, 0x20);
    func_8010C1EC(D_8010DA90, 0x20);
    return source;
}
