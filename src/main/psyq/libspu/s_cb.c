#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_spu_internal.h"

void _SpuCallback(SpuCallback callback) {
    InterruptCallback(9, callback);
}
