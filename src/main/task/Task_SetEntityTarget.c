extern char *g_PlayerEntity;
extern char *g_FieldActorListHead;

void Scene_FreeEntityTable(void *arg0);

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
