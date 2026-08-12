
#include "pe1/psyq_api_internal.h"

int StartRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    D_8009B7CC->mask |= _interrupt_status_masks[id];
    return id < 3;
}
