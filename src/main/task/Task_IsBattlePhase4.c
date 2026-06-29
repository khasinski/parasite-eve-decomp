extern int g_RenderStateFlags;

int Task_IsBattlePhase4(int **arg0) {
    *arg0[0] = ((g_RenderStateFlags & 7) == 4);
    return 1;
}
