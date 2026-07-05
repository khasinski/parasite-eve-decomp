extern int *g_CurrentEntity;

int Pm_Stop(int arg0, int arg1, int arg2);

extern char *g_PlayerEntity;
extern char *g_FieldActorListHead;

void Scene_FreeEntityTable(void *arg0);

void Pm_StopAllBoth(void);

int Menu_SetSlotEntry(int index, unsigned int limit, unsigned int slot);

int Pm_ScriptStopCurrentEntity(int **arg0) {
    Pm_Stop(*arg0[0], g_CurrentEntity, 0);
    return 1;
}

int Task_SetEntityTarget(int **arg0) {
    int **args = arg0;
    char *selected;


    if (*args[0] == 0) {
        selected = g_PlayerEntity;
    } else {
        int id = *args[0];

        selected = g_FieldActorListHead;
        while (selected != 0) {
            if ((unsigned char)selected[0xC] == id &&
                    (unsigned char)selected[0xD] == *args[1] &&
                    ((*(int *)(selected + 0x98) & 0x10) == 0)) {
                break;
            }
            selected = *(char **)(selected + 4);
        }
    }

    Scene_FreeEntityTable(selected);
    return 1;
}

int Task_ResetRenderState(void) {
    Pm_StopAllBoth();
    return 1;
}

int Task_SetMenuSlotEntry(int **arg0) {
    Menu_SetSlotEntry(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}
