
int Gpu_BuildDrawModeCmd(int arg0, int arg1, int arg2) {
    int bits;
    int cmd;

    cmd = 0xE1000000;
    if (arg1 != 0) {
        cmd = 0xE1000200;
    }
    bits = arg2 & 0x9FF;
    if (arg0 != 0) {
        bits |= 0x400;
        return cmd | bits;
    }
    return bits | cmd;
}
