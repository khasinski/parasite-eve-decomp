extern int g_McOpPending;

int MemCard_IsOperationPending(void) {
    return g_McOpPending;
}
