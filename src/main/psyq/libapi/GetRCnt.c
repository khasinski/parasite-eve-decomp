/* MASPSX_FLAGS: --store-return-delay */

#include "pe1/psyq_api_internal.h"

int GetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    return _counters[id].rootCounter;
}
