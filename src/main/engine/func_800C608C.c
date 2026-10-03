#include "common.h"
void func_800C608C(int scale, u8 *src, u8 *dst) {
    scale = (short)scale;

    if (scale != 0x80) {
        int value;

        value = src[0];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[0] = value;

        value = src[1];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[1] = value;

        value = src[2];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[2] = value;
    } else {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }
}
