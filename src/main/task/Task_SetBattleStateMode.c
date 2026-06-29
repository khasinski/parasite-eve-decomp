extern int g_RenderStateFlags;

int Task_SetBattleStateMode(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr = (*ptr & -8) | 0x80;
    return 1;
}
