extern unsigned short D_800BCFE8;
extern unsigned short g_ScreenTransitionTargetY;
extern unsigned short D_800BCFEC;
extern unsigned char g_ScreenTransitionState;
extern unsigned char g_ScreenTransitionMode;
extern unsigned short D_800BCFF0;
extern unsigned short g_ScreenTransitionStartY;
extern unsigned short D_800BCFF4;
extern unsigned short g_ScreenTransitionFadeColor;
extern unsigned short D_800BCFF8;

asm(".globl func_80066B60");
asm("func_80066B60 = CdRom_SetSeekPos");

int CdRom_SetSeekPos(unsigned int arg0) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;

    old0 = D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = D_800BCFEC;

    D_800BCFE8 = 0xFF;
    g_ScreenTransitionTargetY = 0xFF;
    D_800BCFEC = 0xFF;
    g_ScreenTransitionState = 2;
    g_ScreenTransitionMode = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    return 0;
}
