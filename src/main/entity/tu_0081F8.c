extern int g_FieldMoveLock;

int Task_ClearBit(int **arg0) {
    *arg0[0] &= ~(1 << *arg0[1]);
    return 1;
}

int Task_TestBit(int **arg0) {
    *arg0[2] = *arg0[0] & (1 << *arg0[1]);
    return 1;
}

int Task_SetBit(int **arg0) {
    *arg0[0] |= 1 << *arg0[1];
    return 1;
}

int Entity_ClearGlobalFlags(int **arg0) {
    g_FieldMoveLock &= ~*arg0[0];
    return 1;
}

int Entity_GetGlobalFlags(int **arg0) {
    *arg0[0] = g_FieldMoveLock;
    return 1;
}

int Entity_SetGlobalFlags(int **arg0) {
    g_FieldMoveLock |= *arg0[0];
    return 1;
}
