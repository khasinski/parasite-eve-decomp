/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_TaskNodePool;

int Entity_SetCurrentNodeActive(int **arg0) {
    *(int *)(g_TaskNodePool + 0x10) = *(unsigned short *)arg0[0];
    return 0;
}
