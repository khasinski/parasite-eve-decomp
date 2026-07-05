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

int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;
    int mode;

    old0 = D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = D_800BCFEC;

    D_800BCFE8 = arg1;
    g_ScreenTransitionTargetY = arg2;
    D_800BCFEC = arg3;
    g_ScreenTransitionState = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    mode = arg4;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    if (mode >= 4) {
        goto fail;
    }
    if (mode < 0) {
        goto fail;
    }

    g_ScreenTransitionMode = arg4;
    goto done;

fail:
    g_ScreenTransitionMode = 1;

done:
    return 0;
}
