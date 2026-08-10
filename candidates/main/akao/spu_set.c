/* MASPSX_FLAGS: --store-return-delay */

extern int D_8009B38C;
extern int D_8009B418;

typedef void (*SpuTransferCallbackProc)(void);

extern SpuTransferCallbackProc _spu_transferCallback;

int Spu_SetReverbMode(int arg0) {
    int ret;

    switch (arg0) {
    case 0:
        ret = 0;
        break;
    case 1:
        ret = 1;
        break;
    default:
        ret = 0;
        break;
    }

    D_8009B38C = arg0;
    D_8009B418 = ret;
    return ret;
}

SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc callback) {
    SpuTransferCallbackProc old = _spu_transferCallback;

    if (callback != old) {
        _spu_transferCallback = callback;
    }
    return old;
}
