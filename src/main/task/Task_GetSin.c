extern char *g_CurrentEntity;

int rsin(int arg0);
int rcos(int arg0);
int Gte_Atan2(int arg0, int arg1);
int Math_IntSqrt(int arg0);

int Task_GetSin(int **arg0) {
    *arg0[1] = rsin(*arg0[0]) << 4;
    return 1;
}

int Task_GetCos(int **arg0) {
    *arg0[1] = rcos(*arg0[0]) << 4;
    return 1;
}

int Task_GetAtan2(int **arg0) {
    *arg0[2] = Gte_Atan2(*arg0[0], *arg0[1]);
    return 1;
}

int Task_GetSqrt(int **arg0) {
    *arg0[1] = Math_IntSqrt(*arg0[0]);
    return 1;
}

int Task_SetEntityFlag1000000(void) {
    *(int *)(g_CurrentEntity + 0x98) |= 0x1000000;
    return 1;
}
