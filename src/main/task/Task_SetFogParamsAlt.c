void Render_SetScrollMode(int arg0, int arg1, unsigned int arg2, int arg3);

int Task_SetFogParamsAlt(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 8);
    return 1;
}
