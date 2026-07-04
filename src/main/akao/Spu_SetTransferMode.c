/* MASPSX_FLAGS: --stack-return-delay --store-call-delay */

extern int _spu_IRQCallback;
void _SpuCallback(int arg0);

int Spu_SetTransferMode(int arg0) {
    int old = _spu_IRQCallback;

    if (arg0 != old) {
        _SpuCallback(_spu_IRQCallback = arg0);
    }
    return old;
}
