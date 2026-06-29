extern char *g_CurrentEntity;

int Scene_LoadRoomAssets(int arg0, void *arg1);

int Task_GetEntityEffect(int **arg0) {
    int value;

    value = Scene_LoadRoomAssets(*arg0[0], g_CurrentEntity);
    *arg0[1] = value;
    return 1;
}
