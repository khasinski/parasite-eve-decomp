void Render_SetEntryVisible(int arg0, int arg1);

int Task_SetRenderEntryEnabled(int **arg0) {
    Render_SetEntryVisible(*arg0[0], *arg0[1]);
    return 1;
}
