extern char SceneE08_CallbackDone[];

int func_80198290(char *obj) {
    int minus_one;

    minus_one = -1;
    obj[0x16] = minus_one;
    obj[0x17] = minus_one;
    obj[0x18] = minus_one;
    obj[0x19] = 3;
    *(void **)(obj + 0xC) = SceneE08_CallbackDone;
    *(int *)(obj + 0x10) = 0;
    *(short *)(obj + 0x14) = 0;
    obj[0x1A] = 0;
    *(short *)(obj + 0x46) = 0;
    return 0;
}
