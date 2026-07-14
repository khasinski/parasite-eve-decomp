void func_80192814(void);

int func_80192550(signed char *obj) {
    obj[3] = 1;
    obj[0x16] = -1;
    obj[0x17] = -1;
    obj[0x18] = -1;
    obj[0x19] = 3;
    *(int *)(obj + 0x88) = 0x400;
    *(void **)(obj + 0xC) = func_80192814;
    *(int *)(obj + 0x10) = 0;
    *(short *)(obj + 0x14) = 0;
    obj[0x1A] = 0;
    *(short *)(obj + 0x94) = 0;
    *(int *)(obj + 0x80) = 0;
    *(int *)(obj + 0x84) = 0;
    *(int *)(obj + 0x7C) = 0;
    *(int *)(obj + 0x3C) = 0;
    *(int *)(obj + 0x40) = 0;
    *(int *)(obj + 0x44) = 0;
    *(short *)(obj + 0x6C) = 0;
    *(short *)(obj + 0x6E) = 0;
    *(short *)(obj + 0x70) = 0;
    obj[0x19] = 0;
    return 0;
}
