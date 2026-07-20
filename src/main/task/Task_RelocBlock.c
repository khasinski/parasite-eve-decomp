/* CC1_FLAGS: -G8 -fno-strength-reduce */
/* MASPSX_FLAGS: -G8 */

#include "pe1/reloc_block.h"

RelocBlock *Task_RelocBlock(RelocBlock *block) {
    RelocU32 offset;
    RelocU32 limit;
    RelocU32 isAbsolute;
    RelocU32 i;
    RelocU32 count;
    register RelocU32 *ptr;
    char frame[8];

    offset = block->u0.baseOffset;
    limit = 0x80000000U;
    g_SceneDataTable1 = block;
    isAbsolute = limit < offset;
    if (!isAbsolute) {
        goto relocate;
    }

    return block;

relocate:
    i = 0;
    count = block->count;
    block->u0.baseOffset = (RelocU32)((char *)block + offset);
    if (count != 0) {
        ptr = (RelocU32 *)block;
        do {
            ptr[2] = ptr[2] + (unsigned int)block;
            i++;
            ptr++;
        } while (i < block->count);
    }

    return g_SceneDataTable1;
}
