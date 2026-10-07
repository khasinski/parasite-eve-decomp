typedef signed short s16;

void func_800CE870(char *arg0, int arg1, s16 *arg2) {
    char *ptr;

    switch (arg1) {
    case 0:
        ptr = *(char **)(arg0 + 0x238);
        arg2[0] = *(int *)(ptr + 0x14);
        ptr = *(char **)(arg0 + 0x238);
        arg2[1] = *(int *)(ptr + 0x18);
        ptr = *(char **)(arg0 + 0x238);
        arg2[2] = *(int *)(ptr + 0x1C);
        break;
    case 1: {
        int value;

        value = *(s16 *)(arg0 + 0x2A);
        arg2[0] = value;
        value = *(s16 *)(arg0 + 0x2E);
        arg2[1] = value;
        value = *(s16 *)(arg0 + 0x32);
        arg2[2] = value;
        break;
    }
    }
}
