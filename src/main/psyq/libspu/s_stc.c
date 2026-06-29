
typedef void (*SpuTransferCallbackProc)(void);

extern SpuTransferCallbackProc _spu_transferCallback;

SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc callback) {
    SpuTransferCallbackProc old = _spu_transferCallback;

    if (callback != old) {
        _spu_transferCallback = callback;
    }
    return old;
}
