extern int g_GameStateFlags;

extern int g_BattleModeState;

extern char *g_CurrentEntity;

void *Battle_GetContextField(int arg0);
void *Battle_GetEnemyContextField(void *arg0, int arg1);

int Scene_SetGlobalFlags(int **arg0) {
    g_GameStateFlags |= *arg0[0];
    return 1;
}

int Scene_ClearGlobalFlags(int **arg0) {
    g_GameStateFlags &= ~*arg0[0];
    return 1;
}

int Menu_SetModeState5(void) {
    g_BattleModeState = 5;
    return 1;
}

int Menu_SetModeState6(void) {
    g_BattleModeState = 6;
    return 1;
}

int Entity_GetField(char ***arg0) {
    register char *value asm("$2");
    char *ctx = g_CurrentEntity;

    if (*(unsigned char *)(ctx + 0xC) == 0) {
        value = Battle_GetContextField(*(unsigned char *)arg0[0]);
    } else {
        value = Battle_GetEnemyContextField(ctx, *(unsigned char *)arg0[0]);
    }

    *arg0[1] = (char *)value;
    return 1;
}
