extern short D_8019A82A;
extern short D_8019A82C;
extern char D_8019A831;

char *func_80199D2C(void *arg0, char arg1, short arg2, short arg3) {
    char *ptr = &D_8019A831;

    *ptr = arg1;
    D_8019A82A = arg2;
    D_8019A82C = arg3;
    return ptr - 0x29;
}
