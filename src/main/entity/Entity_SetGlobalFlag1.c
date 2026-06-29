extern int g_FieldMoveLock;

int Entity_SetGlobalFlag1(void) {
    g_FieldMoveLock |= 1;
    return 1;
}
