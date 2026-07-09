extern int D_800B8AB0;

int DsStartCallback(int callback) {
    int *slot;
    int old;

    slot = &D_800B8AB0;
    old = *slot;
    *slot = callback;
    return old;
}
