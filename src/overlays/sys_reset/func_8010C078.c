#include "pe1/sys_reset.h"

s32 func_8010C078(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C39C();
    } else {
        result = ((u32)func_8010C430() >> 24) & 1;
    }
    return result;
}
