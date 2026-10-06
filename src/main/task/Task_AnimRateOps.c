extern char *g_CurrentEntity;
extern char *g_PlayerEntity;
extern char g_PlayerBattleEntity[];
extern unsigned short D_800B0D88;

void Anim_SetInterpRate(void *arg0, int arg1);

extern int g_BattleModeState;

int Task_Noop3(void) {
    return 1;
}

int Task_Noop4(void) {
    return 1;
}

int Task_SetAnimInterpRate(short **arg0) {
    short **args = arg0;
    char *current;
    char *selected;
    unsigned short flags;

    Anim_SetInterpRate(g_CurrentEntity + 0x1B4, *args[0]);

    current = g_CurrentEntity;
    flags = *(unsigned short *)(current + 0x250);
    selected = g_PlayerEntity;
    *(unsigned short *)(current + 0x250) = flags | 2;

    if (current == selected) {
        Anim_SetInterpRate(g_PlayerBattleEntity, *args[0]);
        D_800B0D88 |= 2;
    }

    return 1;
}

int Menu_GetModeState(int **arg0) {
    *arg0[0] = g_BattleModeState;
    return 1;
}
