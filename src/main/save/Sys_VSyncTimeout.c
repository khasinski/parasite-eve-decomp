/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */

extern int VSync(int arg0);
extern int CdRom_SendQueuedCmd(int arg0);
extern void Save_ProcessDataCallback(void);

extern int D_8009B6C4;

int Sys_VSyncTimeout(int arg0) {
    int v0;
    int s0;
    int *state;

    v0 = VSync(-1);
    state = &D_8009B6C4;
    asm volatile("" : "=r"(state) : "0"(state));

    if ((state[0] + 0x4B0) < v0) {
        Save_ProcessDataCallback();
        s0 = -1;
    } else {
        s0 = state[-4];
    }

    CdRom_SendQueuedCmd(arg0);
    return s0;
}
