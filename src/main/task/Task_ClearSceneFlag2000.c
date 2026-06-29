extern int g_GameState;

int Task_ClearSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr &= -0x2001;
    return 1;
}
