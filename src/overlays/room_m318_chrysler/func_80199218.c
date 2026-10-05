extern int D_801998F0;
extern int D_801998F4;

int* func_80199218(void* arg0, int arg1, int arg2) {
    int* base = &D_801998F0;

    *base = arg2;
    D_801998F4 = arg1;

    return base;
}
