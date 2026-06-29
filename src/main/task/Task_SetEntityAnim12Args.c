extern char *g_CurrentEntity;

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
