
#include "pe1/psyq_api_internal.h"

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

#include "pe1/psyq_api_internal.h"

int GetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    return _counters[id].rootCounter;
}

#include "pe1/psyq_api_internal.h"

int StartRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    D_8009B7CC->mask |= _interrupt_status_masks[id];
    return id < 3;
}

#include "pe1/psyq_api_internal.h"

int StopRCnt(unsigned int counter) {
    int offset;
    int mask;
    register InterruptControlRegisters *status asm("$5");
    int value;
    offset = (counter & 0xFFFF) << 2;
    status = D_8009B7CC;
    /* Match the original %hi/%lo indexed mask load without using $at. */
    asm volatile(
        ".set\tnoreorder\n\t"
        "lui\t$2,%%hi(D_8009B7D4)\n\t"
        "addu\t$2,$2,$4\n\t"
        "lw\t$2,%%lo(D_8009B7D4)($2)\n\t"
        "lw\t$3,4($5)\n\t"
        "nor\t$2,$zero,$2\n\t"
        "and\t$3,$3,$2\n\t"
        ".set\treorder"
        : "=r"(mask), "=r"(value)
        : "r"(offset), "r"(status));
    status->mask = value;
    return 1;
}

#include "pe1/psyq_api_internal.h"

int ResetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    D_8009B7D0[id].rootCounter = 0;
    return 1;
}
