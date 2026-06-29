extern int g_FieldMoveLock;

int Task_SetInputFlagBit2(void) {
    g_FieldMoveLock |= 4;
    return 1;
}
