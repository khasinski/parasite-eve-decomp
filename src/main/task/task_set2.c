void Render_SetScrollMode(int arg0, int arg1, unsigned int arg2, int arg3);

extern int g_RenderStateFlags;

int Task_SetFogParams(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 0);
    return 1;
}

int Task_SetFogParamsAlt(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 8);
    return 1;
}

int Task_IsBattlePhase4(int **arg0) {
    *arg0[0] = ((g_RenderStateFlags & 7) == 4);
    return 1;
}
