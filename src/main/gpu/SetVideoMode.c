/* MASPSX_FLAGS: --store-return-delay */

extern int D_800956EC;

int SetVideoMode(int mode) {
    int old;

    old = D_800956EC;
    D_800956EC = mode;
    return old;
}
