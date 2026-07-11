void *func_800C2B50(void);

void func_80194C0C(void *arg0, void *arg1, char *arg2) {
    char *position = func_800C2B50();

    *(int *)(arg2 + 0x0) = *(int *)(position + 0x18) - 0x15E;
    *(int *)(arg2 + 0x4) = *(int *)(position + 0x1C);
    *(int *)(arg2 + 0x8) = *(int *)(position + 0x20);
    *(short *)(arg2 + 0x10) = 0;
    *(short *)(arg2 + 0x12) = 0;
    *(short *)(arg2 + 0x14) = 0xFF;
}
