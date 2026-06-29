extern unsigned char g_MemCardPortStates[];
extern int g_MemCardReadContext;

void MenuWidget_SaveAndSetCurrentNode(int arg0);

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
