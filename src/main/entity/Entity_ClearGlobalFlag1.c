extern int g_FieldMoveLock;

int Entity_ClearGlobalFlag1(void) {
    g_FieldMoveLock &= -2;
    return 1;
}
