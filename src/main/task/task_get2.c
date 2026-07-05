extern int g_CombatModeFlags;

extern char *g_CurrentEntity;

int Scene_LoadRoomAssets(int arg0, void *arg1);

int Task_GetCombatModeFlag(int **arg0) {
    if (g_CombatModeFlags & 4) {
        *arg0[0] = 1;
    } else {
        *arg0[0] = 0;
    }

    return 1;
}

int Task_SetCombatModeFlag(void) {
    int *ptr = &g_CombatModeFlags;

    *ptr |= 4;
    return 1;
}

int Task_GetEntityEffect(int **arg0) {
    int value;

    value = Scene_LoadRoomAssets(*arg0[0], g_CurrentEntity);
    *arg0[1] = value;
    return 1;
}
