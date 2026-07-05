/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern char *g_TaskNodePool;

int Entity_SetCurrentNodeActive(int **arg0) {
    *(int *)(g_TaskNodePool + 0x10) = *(unsigned short *)arg0[0];
    return 0;
}

int Entity_MarkCurrentNodeFree(void) {
    *(unsigned short *)(g_TaskNodePool + 8) |= 0x10;
    return 0;
}
