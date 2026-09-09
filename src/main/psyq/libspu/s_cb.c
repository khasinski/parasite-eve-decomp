#include "pe1/psyq_callbacks.h"
typedef void (*SpuCallback)(void);


void _SpuCallback(SpuCallback callback) {
    InterruptCallback(9, callback);
}
