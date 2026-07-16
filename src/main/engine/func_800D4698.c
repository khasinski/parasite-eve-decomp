extern void *D_800F32D0;
extern void *D_800E2368;

int func_800D4698(char *obj, int skip, int arg2, int arg3, int arg4, int arg5) {
    void *ctx;
    int (*callback)(int, int, int, int);

    ctx = obj + 0xC;
    if (skip == 0) {
        callback = *(int (**)(int, int, int, int))(*(char **)(obj + 0x8C) + 0x30);
        D_800F32D0 = obj;
        D_800E2368 = ctx;
        if (callback != 0) {
            *(int *)(obj + 0x14) = callback(arg2, arg3, arg4, arg5);
        }
    }
    return 0;
}
