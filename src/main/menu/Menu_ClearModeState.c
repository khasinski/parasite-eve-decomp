extern int g_BattleModeState;

int Menu_ClearModeState(void) {
    g_BattleModeState = 0;
    return 1;
}
