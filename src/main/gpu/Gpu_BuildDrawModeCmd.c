/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int Gpu_BuildDrawModeCmd(int arg0, int arg1, int arg2) {
    register int bits asm("$2");
    int cmd;

    cmd = 0xE1000000;
    if (arg1 != 0) {
        cmd = 0xE1000200;
    }
    bits = arg2 & 0x9FF;
    if (arg0 != 0) {
        bits |= 0x400;
    }
    return cmd | bits;
}
