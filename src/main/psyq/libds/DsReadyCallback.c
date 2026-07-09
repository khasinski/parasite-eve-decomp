extern int D_800B8AB8;

int DsReadyCallback(int callback) {
    int *slot;
    int old;

    slot = &D_800B8AB8;
    old = *slot;
    *slot = callback;
    return old;
}
