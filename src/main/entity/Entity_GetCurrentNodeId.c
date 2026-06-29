/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_TaskNodePool;

int Entity_GetCurrentNodeId(int **arg0) {
    *arg0[0] = *(unsigned short *)(g_TaskNodePool + 0xA);
    return 1;
}
