typedef void (*DsCallback)(void);

extern DsCallback D_8009B708;

DsCallback DsReadCallback(DsCallback callback) {
    DsCallback *slot;
    DsCallback old;

    slot = &D_8009B708;
    old = *slot;
    *slot = callback;
    return old;
}
