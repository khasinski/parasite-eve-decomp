/* MASPSX_FLAGS: --store-return-delay */

#include "pe1/psyq_api_internal.h"

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
