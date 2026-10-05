void RoomLib_WindowHandler(void);

int func_8018F05C(char *ctx) {
    int value;

    ctx[3] = 1;
    value = -1;
    ctx[0x16] = value;
    ctx[0x17] = value;
    ctx[0x18] = value;
    ctx[0x19] = 3;
    *(int *)(ctx + 0x88) = 0x400;
    *(void (**)(void))(ctx + 0xC) = RoomLib_WindowHandler;
    *(int *)(ctx + 0x10) = 0;
    *(short *)(ctx + 0x14) = 0;
    ctx[0x1A] = 0;
    *(short *)(ctx + 0x94) = 0;
    *(int *)(ctx + 0x80) = 0;
    *(int *)(ctx + 0x84) = 0;
    *(int *)(ctx + 0x7C) = 0;
    *(int *)(ctx + 0x3C) = 0;
    *(int *)(ctx + 0x40) = 0;
    *(int *)(ctx + 0x44) = 0;
    *(short *)(ctx + 0x6C) = 0;
    *(short *)(ctx + 0x6E) = 0;
    *(short *)(ctx + 0x70) = 0;
    ctx[0x19] = 0;

    return 0;
}
