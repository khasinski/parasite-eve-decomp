/* CC1_FLAGS: -G8 -fno-strength-reduce */
/* MASPSX_FLAGS: -G8 */

/* Task-node pool and scene relocation-block setup. Pool initialization,
 * scene data relocation, the scene-map flush and free-list construction are
 * contiguous and share the node pool and g_SceneDataTable1 state. */

#include "pe1/reloc_block.h"

extern int g_TaskNodeFreeListHead;
extern int g_SceneDataTable0;
extern int g_TaskNodePool;
extern short g_TaskNodeSeqCounter;
extern int D_8009D310[72][11];
extern int D_8009D334[72][11];
extern int D_8009DF68[];
extern int g_TaskNodeActiveFlags[64];
extern int g_TaskScriptOperandTable[16];

int DrawSync(int arg0);
void Scene_LoadMap();

void Task_InitNodePool(void) {
    unsigned int i;
    unsigned int j;
    unsigned int rowOffset;
    char *base;
    int *ptr;

    i = 0;
    base = (char *)D_8009D310;
    rowOffset = 0;

    g_TaskNodePool = 0;
    g_TaskNodeSeqCounter = 0;
    g_TaskNodeFreeListHead = 0;

    for (; i < 72; i++, rowOffset += 0x2C) {
        register unsigned int colOffset;
        int *cell;

        j = 0;
        colOffset = rowOffset;
        for (; j < 11; j++, colOffset += 4) {
            cell = (int *)(colOffset + (unsigned int)base);
            asm volatile("" : "=r"(cell) : "0"(cell));
            *cell = 0;
        }
    }

    g_SceneDataTable0 = 0;

    i = 0;
    ptr = g_TaskScriptOperandTable;
    while (i < 16) {
        *ptr++ = 0;
        i++;
    }

    g_SceneDataTable1 = 0;
}

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

void Task_DrawSyncAndFlush(void) {
    unsigned int i;
    register int offset asm("$17");
    RelocBlock *block;
    unsigned int count;
    char frame[8];

    DrawSync(0);
    block = g_SceneDataTable1;
    count = (unsigned char)block->u0.base[0];
    if (count != 0) {
        i = 0;
        offset = 1;
        do {
            Scene_LoadMap(block->u0.base + offset, 0, 1);
            block = g_SceneDataTable1;
            i++;
            offset += 2;
        } while (i < (unsigned char)block->u0.base[0]);
    }
}

void Task_InitNodeFreeList(void) {
    unsigned short i;

    i = 0;
    g_TaskNodePool = 0;
    g_TaskNodeFreeListHead = (int)D_8009D310;

    do {
        D_8009D334[i][0] = (int)&D_8009D310[i + 1][0];
        i++;
    } while (i < 0x47);

    D_8009DF68[0] = 0;

    i = 0;
    do {
        g_TaskNodeActiveFlags[i] = 0;
        i++;
    } while (i < 0x40);
}
