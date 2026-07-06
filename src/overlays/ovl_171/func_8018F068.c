extern int D_8018F160;

int func_8018F068(unsigned char *arg0) {
    *(int **)(arg0 + 0xC) = &D_8018F160;
    arg0[3] = 0xFF;
    arg0[0x1A] = 0;
    arg0[0x28] = 0;
    return 0;
}
