extern volatile int g_MemCardActivePortOneBased;
extern volatile int g_MemCardActivePromptPending;

void MemCard_ClearActivePrompt(void) {
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}
