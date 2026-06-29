extern int g_CombatModeFlags;

int Task_SetCombatModeFlag(void) {
    int *ptr = &g_CombatModeFlags;

    *ptr |= 4;
    return 1;
}
