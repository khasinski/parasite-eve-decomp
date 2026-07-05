/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void Tbl_ClearEntry(int arg0);

extern char *g_TaskNodePool;

int Task_ClearTableEntry(short **arg0) {
    Tbl_ClearEntry(*arg0[0]);
    return 1;
}

int Task_GetNodeTargets18_1C(int **arg0) {
    *arg0[0] = *(int *)(g_TaskNodePool + 0x18);
    *arg0[1] = *(int *)(g_TaskNodePool + 0x1C);
    return 1;
}
