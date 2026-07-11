/* MASPSX_FLAGS: --store-return-delay */

extern int D_8009B6D0;

int func_80081254(int callback) {
    int old;

    old = D_8009B6D0;
    D_8009B6D0 = callback;
    return old;
}
