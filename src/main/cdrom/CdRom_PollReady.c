int Sys_VSyncTimeout(int arg0);

extern int g_GameState;

int CdRom_PollReady(void) {
    int scratch;
    int status;
    int *state;

    status = Sys_VSyncTimeout((int)&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = &g_GameState;
        *state &= 0xFEFFBFFF;
    }
    return status;
}
