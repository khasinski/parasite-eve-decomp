extern char *g_CurrentEntity;

void Task_SetObjAnimEntry5(void *arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

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
