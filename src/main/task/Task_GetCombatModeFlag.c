extern int g_CombatModeFlags;

int Task_GetCombatModeFlag(int **arg0) {
    if (g_CombatModeFlags & 4) {
        *arg0[0] = 1;
    } else {
        *arg0[0] = 0;
    }

    return 1;
}
