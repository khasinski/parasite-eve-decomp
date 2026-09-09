#include "pe1/psyq_spu_internal.h"

SpuCallback SpuSetTransferCallback(SpuCallback callback) {
    SpuCallback old = _spu_transferCallback;

    if (callback != old) {
        _spu_transferCallback = callback;
    }

    return old;
}
