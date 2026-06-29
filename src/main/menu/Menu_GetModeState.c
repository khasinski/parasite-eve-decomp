extern int g_BattleModeState;

int Menu_GetModeState(int **arg0) {
    *arg0[0] = g_BattleModeState;
    return 1;
}
