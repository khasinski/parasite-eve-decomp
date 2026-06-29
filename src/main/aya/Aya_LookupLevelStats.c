extern int D_800A803C[];

void *Aya_LookupLevelStats(int arg0) {
    if (arg0 < 0) {
        arg0 = 0;
    } else if (arg0 >= 0x63) {
        arg0 = 0x62;
    }

    return (void *)(D_800A803C[0] + (int)((char *)D_800A803C - 0x14) + (arg0 * 0x18));
}
