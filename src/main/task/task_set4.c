int Gpu_LoadGeomState(int index);

extern short g_CameraBaseAngleX;
extern short g_CameraBaseAngleY;

int CdRom_SetSeekPos(unsigned int arg0);

int Render_SetFadeColour(unsigned int arg0);

int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4);

extern unsigned char g_ScreenTransitionState;

extern char *g_CurrentEntity;
extern char *g_PlayerEntity;
extern short D_800BCFFE;

int Task_LoadGeomState(int **arg0) {
    Gpu_LoadGeomState(**arg0);
    return 1;
}

int Task_SetScreenScrollPos(int **arg0) {
    g_CameraBaseAngleX = *arg0[0];
    asm volatile("" ::: "memory");
    g_CameraBaseAngleY = *arg0[1];
    return 1;
}

int Task_SetCdRomSeekPos(unsigned short **arg0) {
    CdRom_SetSeekPos(**arg0);
    return 1;
}

int Task_SetFogColor(unsigned short **arg0) {
    Render_SetFadeColour(**arg0);
    return 1;
}

int Task_SetCdRomScreenPos(unsigned short **arg0) {
    return CdRom_SetScreenPos(*arg0[0], *arg0[1], *arg0[2], *arg0[3], *arg0[4]), 1;
}

int Task_ClearEntityFlag40(void) {
    unsigned char *ptr = &g_ScreenTransitionState;

    *ptr &= 0xBF;
    return 1;
}

int Task_SetEntityMoveSpeed(int **arg0) {
    int **args = arg0;
    char *current = g_CurrentEntity;
    char *selected = g_PlayerEntity;
    int value = *args[0] >> 4;

    *(short *)(current + 0x26) = value;
    if (current == selected) {
        int scaled;

        scaled = ((*args[0] >> 4) * 3) << 7;

        if (scaled < 0) {
            scaled += 0xFFF;
        }
        D_800BCFFE = scaled >> 12;
    }
    return 1;
}
