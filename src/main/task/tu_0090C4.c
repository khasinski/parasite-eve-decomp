extern char *g_CurrentEntity;

int Geo_PointInPoly(int arg0, int arg1, int *arg2, int arg3);

void Entity_AllocSlot(void *arg0);

void Task_SetObjAnimEntry12(
    void *arg0,
    int arg1,
    int arg2,
    int arg3,
    int arg4,
    int arg5,
    int arg6,
    int arg7,
    int arg8,
    int arg9,
    int arg10,
    int arg11);

void Task_SetObjAnimEntry5(void *arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

extern char * volatile g_GeomState;

void Geo_TransformPoint(void *arg0, int arg1, int arg2, int arg3);

void Render_SetEntryVisible(int arg0, int arg1);

void Geo_ClipPoint(int arg0, int arg1, int arg2);

void Obj_SetEntryLimit(int arg0, int arg1);

void Sys_SetFlagSlot(int arg0, int arg1);

int Task_PlayerPointInPoly(int **arg0) {
    int values[8];
    unsigned int i;

    for (i = 0; i < 4; i++) {
        values[i * 2] = *arg0[i * 2];
        values[i * 2 + 1] = *arg0[i * 2 + 1];
    }

    *arg0[8] = Geo_PointInPoly(
        *(int *)(g_CurrentEntity + 0x28),
        *(int *)(g_CurrentEntity + 0x30),
        values,
        4);
    return 1;
}

int Task_InitEntityMoveState(void) {
    Entity_AllocSlot(g_CurrentEntity);
    return 1;
}

int Task_SetEntityAnim12Args(char **arg0) {
    Task_SetObjAnimEntry12(
        g_CurrentEntity,
        *(unsigned char *)arg0[0],
        *(unsigned char *)arg0[1],
        *(unsigned char *)arg0[2],
        *(unsigned char *)arg0[3],
        *(unsigned short *)arg0[4],
        *(signed char *)arg0[5],
        *(signed char *)arg0[6],
        *(signed char *)arg0[7],
        *(signed char *)arg0[8],
        *(unsigned char *)arg0[9],
        *(unsigned char *)arg0[10]);
    return 1;
}

int Task_SetEntityAnim5Args(char **arg0) {
    Task_SetObjAnimEntry5(
        g_CurrentEntity,
        *(unsigned char *)arg0[0],
        *(unsigned char *)arg0[1],
        *(unsigned char *)arg0[2],
        *(unsigned char *)arg0[3],
        *(unsigned short *)arg0[4]);
    return 1;
}

int Task_SetSceneEntryAnim(int **arg0) {
    char *header = g_GeomState;
    char *base = g_GeomState;
    int index = *arg0[0];

    Geo_TransformPoint(
        base + *(int *)(header + 0x14) + (index * 56),
        *(short *)arg0[1],
        *(short *)arg0[2],
        *(short *)arg0[3]);
    return 1;
}

int Task_SetRenderEntryEnabled(int **arg0) {
    Render_SetEntryVisible(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetRenderEntryPos(short **arg0) {
    Geo_ClipPoint(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int Task_SetObjEntryLimit(int **arg0) {
    Obj_SetEntryLimit(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetFlagSlot(int **arg0) {
    Sys_SetFlagSlot(*arg0[0], *arg0[1]);
    return 1;
}
