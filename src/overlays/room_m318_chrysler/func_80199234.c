extern int D_8019990C;
extern int D_80199910;

int* func_80199234(void* arg0, int arg1, int arg2) {
    int* base = &D_8019990C;

    *base = arg2;
    D_80199910 = arg1;

    return base;
}
