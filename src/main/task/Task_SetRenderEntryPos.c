void Geo_ClipPoint(int arg0, int arg1, int arg2);

int Task_SetRenderEntryPos(short **arg0) {
    Geo_ClipPoint(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}
