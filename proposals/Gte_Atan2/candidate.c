/* MASPSX_FLAGS: --expand-div */
#include "common.h"
extern s16 atan_table[] __asm__("D_8009A6EC");

int Gte_Atan2(int y, int x) {
    int negative_x = 0;
    int negative_y = 0;
    int angle;
    if (x < 0) { negative_x = 1; x = (int)(0u - (u32)x); }
    if (y < 0) { negative_y = 1; y = (int)(0u - (u32)y); }
    if (x == 0 && y == 0) return 0;
    if (y < x) {
        if (y & 0x7FE00000) y = y / (x >> 10);
        else y = (int)((u32)y << 10) / x;
        angle = atan_table[y];
    } else {
        if (x & 0x7FE00000) y = x / (y >> 10);
        else y = (int)((u32)x << 10) / y;
        angle = 1024 - atan_table[y];
    }
    if (negative_x) angle = 2048 - angle;
    if (negative_y) angle = -angle;
    return angle;
}
