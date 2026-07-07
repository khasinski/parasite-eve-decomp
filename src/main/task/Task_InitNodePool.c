/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_TaskNodeFreeListHead;
extern int g_SceneDataTable0;
extern int g_SceneDataTable1;
extern int g_TaskNodePool;
extern short g_TaskNodeSeqCounter;
extern int D_8009D310[72][11];
extern int g_TaskScriptOperandTable[16];

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
