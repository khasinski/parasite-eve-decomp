void func_80194B4C(void *arg0, char *arg1, char *arg2) {
    register int value __asm__("v0");
    register int work __asm__("v1");
    register int delta __asm__("a0");
    register char *ptr __asm__("v1");

    work = *(unsigned short *)(arg2 + 0x1C);
    delta = *(unsigned short *)(arg2 + 0x1E);
    value = *(unsigned char *)(arg2 + 0x22);
    work -= delta;
    *(short *)(arg2 + 0x1C) = work;

    work = *(unsigned short *)(arg2 + 0x1E);
    value += 0x10;
    arg2[0x22] = value;

    value = *(unsigned short *)(arg2 + 0x20);
    work += 0xB;
    value += 0x8;
    *(short *)(arg2 + 0x1E) = work;
    *(short *)(arg2 + 0x20) = value;
    ptr = arg2;

    if ((short)value >= 0x81) {
        *(short *)(arg2 + 0x20) = 0x80;
    }

    if (*(short *)(arg2 + 0x1C) < 0) {
        *(short *)(arg2 + 0x1C) = 0;
    }

    if (*(unsigned char *)(arg2 + 0x22) >= 0xC9) {
        arg2[0x22] = 0xC8;
    }

    value = *(unsigned char *)(ptr + 0x22) >> 4;
    if (value >= 4) {
        *(short *)(ptr + 0x14) = *(unsigned short *)(ptr + 0x14) + 0xC8;
    }

    if (*(short *)(arg1 + 0x2) == 0x78) {
        arg1[1] = 2;
    }
}
