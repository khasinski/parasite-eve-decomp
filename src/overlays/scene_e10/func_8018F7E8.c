char *func_800C2B50(void);

void func_8018F7E8(void *arg0, void *arg1, char *state) {
    char *actor;
    int coord;
    int scale;

    actor = func_800C2B50();
    *(short *)(state + 0x0) = *(int *)(actor + 0x58);
    *(short *)(state + 0x2) = *(int *)(actor + 0x5C);
    *(short *)(state + 0x4) = *(int *)(actor + 0x60);
    *(short *)(state + 0x8) = *(int *)(actor + 0x38);
    *(short *)(state + 0xA) = *(int *)(actor + 0x3C);
    coord = *(int *)(actor + 0x40);
    scale = 0x400;
    *(short *)(state + 0x12) = 0;
    *(short *)(state + 0x10) = scale;
    *(short *)(state + 0xC) = coord;
}
