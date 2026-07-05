extern int g_MemCardActivePortOneBased;
extern int g_MemCardActivePromptPending;

void MemCard_StartRead(int port, int arg1);

typedef unsigned char u8;

extern u8 D_800A0EDE[];

extern unsigned char g_MemCardPortStates[];
extern int g_MemCardReadContext;

void MenuWidget_SaveAndSetCurrentNode(int arg0);

void MemCard_StartActivePortRead(void) {
    MemCard_StartRead(g_MemCardActivePortOneBased - 1, 1);
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}

int MemCard_GetPortFileCount(int arg0) {
    return D_800A0EDE[arg0 * 0x418];
}

void MemCard_StartRead(int port, int arg1) {
    unsigned char *state = &g_MemCardPortStates[port * 0x418];

    if (state[1] == 0 || state[1] == 12) {
        state[1] = 1;
        state[0xB] = 2;
        *(short *)(state + 0x16) = 10;
        MenuWidget_SaveAndSetCurrentNode(0);
        g_MemCardReadContext = arg1;
    }
}
