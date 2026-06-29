extern char * volatile g_GeomState;

void Geo_TransformPoint(void *arg0, int arg1, int arg2, int arg3);

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
