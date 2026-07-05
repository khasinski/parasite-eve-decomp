extern int g_GameState;

int Task_SetSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr |= 0x2000;
    return 1;
}

int Task_ClearSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr &= -0x2001;
    return 1;
}
