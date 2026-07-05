extern int g_BattleModeState;

int Menu_ClearModeState(void) {
    g_BattleModeState = 0;
    return 1;
}

int Menu_SetModeState8(void) {
    g_BattleModeState = 8;
    return 1;
}
