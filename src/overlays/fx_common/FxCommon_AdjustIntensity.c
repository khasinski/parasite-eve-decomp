#include "fx_common.h"

s16 func_80194108(s16 value)
{
    s16 result;

    result = value;
    if (value > 0) {
        if (value < 0xFF) {
            func_801941A4((u8)result);
            result = value + 0x10;
        } else {
            func_801941A4(0xFF);
        }
    }

    if (result < 0) {
        if (result >= -0xFE) {
            func_801941A4((u8)(result - 1));
            result -= 0x10;
        } else {
            func_801941A4(0);
            result = 0;
        }
    }

    return result;
}
