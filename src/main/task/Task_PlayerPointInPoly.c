extern char *g_CurrentEntity;

int Geo_PointInPoly(int arg0, int arg1, int *arg2, int arg3);

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
