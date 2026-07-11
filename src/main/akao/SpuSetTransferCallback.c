typedef void (*SpuTransferCallbackProc)(void);

extern SpuTransferCallbackProc D_8009B434;

SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc callback) {
    SpuTransferCallbackProc old = D_8009B434;

    if (callback != old) {
        D_8009B434 = callback;
    }

    return old;
}
