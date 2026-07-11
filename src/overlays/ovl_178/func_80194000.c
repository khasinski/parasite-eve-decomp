void *func_800C2B50(void);
void *func_800C2B10(int arg0);

void func_80194000(void *arg0, void *arg1, char *arg2) {
    char *position;
    register int value __asm__("v0");
    register int z __asm__("v1");

    position = func_800C2B50();
    func_800C2B10(1);

    value = 0x14;
    *(short *)(arg2 + 0x8C) = value;
    value = 0x80;
    *(short *)(arg2 + 0x88) = value;
    value = 0x400;
    *(short *)(arg2 + 0x8A) = 0;
    *(short *)(arg2 + 0x30) = 0;
    *(short *)(arg2 + 0x32) = value;
    *(short *)(arg2 + 0x34) = 0;

    value = *(int *)(position + 0x18);
    *(int *)(arg2 + 0x0) = value - 0x15E;
    value = *(int *)(position + 0x1C);
    *(int *)(arg2 + 0x4) = value + 0x78;

    z = *(int *)(position + 0x20);
    value = 0x800;
    *(short *)(arg2 + 0x48) = 0;
    *(short *)(arg2 + 0x4A) = value;
    *(short *)(arg2 + 0x4C) = 0;
    *(int *)(arg2 + 0x8) = z;
}
