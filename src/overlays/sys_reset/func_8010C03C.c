#include "common.h"
extern s32 func_8010C308(void);
extern s32 func_8010C430(void);

s32 func_8010C03C(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C308();
    } else {
        result = ((u32)func_8010C430() >> 29) & 1;
    }
    return result;
}
