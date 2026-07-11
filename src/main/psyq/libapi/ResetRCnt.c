/* MASPSX_FLAGS: --store-jump-delay --store-return-delay */

#include "pe1/psyq_api_internal.h"

int ResetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    _counters[id].rootCounter = 0;
    return 1;
}
