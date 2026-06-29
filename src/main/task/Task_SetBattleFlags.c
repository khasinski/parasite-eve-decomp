extern int g_RenderStateFlags;

int Task_SetBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr |= 0xC0;
    return 1;
}
