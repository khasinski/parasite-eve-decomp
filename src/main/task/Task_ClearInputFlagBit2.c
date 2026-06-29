extern int g_FieldMoveLock;

int Task_ClearInputFlagBit2(void) {
    g_FieldMoveLock &= -5;
    return 1;
}
