extern int D_80199914;
extern int D_80199918;
extern int D_8019991C;
extern int D_80199920;

int* func_80199250(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        D_80199914 = arg1;
    } else {
        D_80199918 = arg1;
        D_8019991C = arg2;
        D_80199920 = arg3;
    }

    return &D_80199914;
}
