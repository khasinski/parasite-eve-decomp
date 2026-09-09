#include "pe1/psyq_api_internal.h"
/* Stock GCC 2.7.2; full counter TU proposal, not an exact match. */


/* Psy-Q libapi/counter.c: five contiguous routines in SDK source order. */

int SetRCnt(unsigned int counter, int target, int mode) {
    int id;
    int counter_mode;

    id = counter & 0xFFFF;
    counter_mode = 0x48;
    if (id >= 3) {
        return 0;
    }

    _counters[id].mode = 0;
    _counters[id].target = target;
    if ((unsigned int)id < 2) {
        if (mode & 0x10) {
            counter_mode = 0x49;
        }
        if (!(mode & 1)) {
            counter_mode |= 0x100;
        }
    } else if (id == 2 && !(mode & 1)) {
        counter_mode = 0x248;
    }
    if (mode & 0x1000) {
        counter_mode |= 0x10;
    }

    _counters[id].mode = counter_mode;
    return 1;
}


int GetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    return _counters[id].rootCounter;
}


int StartRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    D_8009B7CC->mask |= _interrupt_status_masks[id];
    return id < 3;
}


int StopRCnt(unsigned int counter) {
    int id = counter & 0xFFFF;
    D_8009B7CC->mask &= ~_interrupt_status_masks[id];
    return 1;
}


int ResetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    D_8009B7D0[id].rootCounter = 0;
    return 1;
}
