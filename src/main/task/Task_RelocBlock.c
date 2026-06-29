/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct RelocBlock {
    unsigned int baseOffset;
    unsigned int count;
    unsigned int offsets[1];
} RelocBlock;

extern RelocBlock *g_SceneDataTable1;

RelocBlock *Task_RelocBlock(RelocBlock *block) {
    unsigned int offset;
    unsigned int limit;
    unsigned int isAbsolute;
    unsigned int i;
    unsigned int count;
    register unsigned int *ptr asm("$3");
    char frame[8];

    offset = block->baseOffset;
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
    block->baseOffset = (unsigned int)((char *)block + offset);
    if (count != 0) {
        ptr = (unsigned int *)block;
        do {
            ptr[2] = ptr[2] + (unsigned int)block;
            i++;
            ptr++;
        } while (i < block->count);
    }

    return g_SceneDataTable1;
}
