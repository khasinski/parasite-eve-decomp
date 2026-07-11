typedef short s16;

extern void *D_800F33E0;

int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4) {
    char *header;
    char *entry;
    int stride;
    int i;

    i = 0;
    stride = arg2 + 4;
    header = *(char **)((char *)D_800F33E0 + 8) + arg1;
    entry = header + 0xC;

    *(char **)arg0 = header;
    *(void **)(header + 8) = arg4;
    *(int *)header = stride;
    *(int *)(header + 4) = arg3;

    while (i < arg3) {
        *(s16 *)entry = 0;
        i++;
        entry += stride;
    }

    return (stride * arg3) + 0xC;
}
