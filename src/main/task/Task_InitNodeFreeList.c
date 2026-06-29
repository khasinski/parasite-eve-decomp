/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_TaskNodeFreeListHead;
extern int g_TaskNodePool;
extern int D_8009D310[72][11];
extern int D_8009D334[72][11];
extern int D_8009DF68[];
extern int g_TaskNodeActiveFlags[64];

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
