/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern char *g_TaskNodePool;

int Task_GetNodeTarget14(int **arg0) {
    *arg0[0] = *(int *)(g_TaskNodePool + 0x14);
    return 1;
}
