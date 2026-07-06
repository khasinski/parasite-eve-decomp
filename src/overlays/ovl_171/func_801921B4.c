extern int D_8019233C;
extern int D_80192340;

int *func_801921B4(int arg0, int arg1, int arg2) {
    int *slot = &D_8019233C;

    *slot = arg1;
    D_80192340 = arg2;
    return slot;
}
