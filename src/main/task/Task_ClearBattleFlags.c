extern int g_RenderStateFlags;

int Task_ClearBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr &= -0xC1;
    return 1;
}
