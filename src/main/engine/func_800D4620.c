int func_800D4620(char *obj) {
    register char *base asm("$6");
    register char *p asm("$5");
    register char *q asm("$4");
    int i;
    int sentinel;

    base = obj + 0xC;
    i = 6;
    p = obj + 0x18;
    obj[2] = 1;
    obj[3] = 0;
    *(short *)(obj + 0x1A) = 0;
    *(short *)(obj + 0x1C) = 0;
    *(char **)(obj + 0x10) = obj + 0x90;
    obj[0x18] = 0;
    obj[0x19] = 0;

    for (; i >= 0; i--) {
        *(short *)(p + 0x12) = 0;
        p -= 2;
    }

    p = base + 0x20;
    i = 0;
    sentinel = 0xFFFF;
    q = base + 0x24;
    for (; i < 8; p += 0xC) {
        i++;
        *(short *)p = sentinel;
        *(short *)(q - 2) = 0;
        *(int *)q = 0;
        q += 0xC;
    }

    return 0;
}
