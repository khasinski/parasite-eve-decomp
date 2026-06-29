void Sys_SetFlagSlot(int arg0, int arg1);

int Task_SetFlagSlot(int **arg0) {
    Sys_SetFlagSlot(*arg0[0], *arg0[1]);
    return 1;
}
