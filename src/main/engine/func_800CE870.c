typedef signed short s16;

void func_800CE870(char *arg0, int arg1, s16 *arg2) {
    char *ptr;

    if (arg1 == 0) {
        goto mode0;
    }
    if (arg1 == 1) {
        goto mode1;
    }
    goto done;

mode0:
    ptr = *(char **)(arg0 + 0x238);
    arg2[0] = *(int *)(ptr + 0x14);
    ptr = *(char **)(arg0 + 0x238);
    arg2[1] = *(int *)(ptr + 0x18);
    ptr = *(char **)(arg0 + 0x238);
    arg2[2] = *(int *)(ptr + 0x1C);
    goto done;

mode1:
    {
        int value;

        value = *(s16 *)(arg0 + 0x2A);
        arg2[0] = value;
        value = *(s16 *)(arg0 + 0x2E);
        arg2[1] = value;
        value = *(s16 *)(arg0 + 0x32);
        arg2[2] = value;
    }

done:
    return;
}
