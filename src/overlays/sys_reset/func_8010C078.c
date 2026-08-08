#include "common.h"
extern s32 func_8010C39C(void);
extern s32 func_8010C430(void);

s32 func_8010C078(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C39C();
    } else {
        result = ((u32)func_8010C430() >> 24) & 1;
    }
    return result;
}
