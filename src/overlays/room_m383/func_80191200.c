void func_80191200(void *arg0, char *arg1, char *arg2) {
    *(unsigned short *)(arg2 + 0x10) += 0x64;
    *(unsigned short *)(arg2 + 0x12) += *(unsigned short *)(arg2 + 0x16);
    *(unsigned short *)(arg2 + 0x16) += 0x32;
    *(unsigned short *)(arg2 + 0x14) -= 0x10;

    if (*(short *)(arg2 + 0x14) < 0) {
        arg1[1] = 2;
        *(unsigned short *)(arg2 + 0x14) = 0;
    }
}
