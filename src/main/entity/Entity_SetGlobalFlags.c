extern int g_FieldMoveLock;

int Entity_SetGlobalFlags(int **arg0) {
    g_FieldMoveLock |= *arg0[0];
    return 1;
}
