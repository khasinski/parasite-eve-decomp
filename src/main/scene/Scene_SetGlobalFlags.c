extern int g_GameStateFlags;

int Scene_SetGlobalFlags(int **arg0) {
    g_GameStateFlags |= *arg0[0];
    return 1;
}
