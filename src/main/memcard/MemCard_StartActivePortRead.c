extern int g_MemCardActivePortOneBased;
extern int g_MemCardActivePromptPending;

void MemCard_StartRead(int port, int arg1);

void MemCard_StartActivePortRead(void) {
    MemCard_StartRead(g_MemCardActivePortOneBased - 1, 1);
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}
