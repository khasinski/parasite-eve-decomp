void func_800C8CBC(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x78;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
