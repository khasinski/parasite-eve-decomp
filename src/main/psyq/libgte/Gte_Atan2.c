/* ASSEMBLER: GNU */
/* GAS_VERSION: 2.7 */
#include "common.h"
extern s16 atan_table[] __asm__("D_8009A6EC");

/* A function section avoids GAS 2.7's mandatory 16-byte .text padding. */
int __attribute__((section(".text.Gte_Atan2"))) Gte_Atan2(int y, int x) {
    int negative_x = 0;
    int negative_y = 0;
    int index;
    register int angle asm("$3");
    int result;
    int y_less_than_x;
    if (x < 0) { negative_x = 1; x = (int)(0u - (u32)x); }
    if (y < 0) { negative_y = 1; y = (int)(0u - (u32)y); }
    y_less_than_x = y < x;
    if (x == 0) {
        result = 0;
        if (y == 0) goto done;
        asm volatile("" : "=r"(x));
        y_less_than_x = y < x;
    }
    if (y_less_than_x) {
        if (y & 0x7FE00000) {
            y = y / (x >> 10);
            index = y << 1;
            /* Keep the shift before the jump; GAS fills its delay slot. */
            asm volatile("" : : "r"(index));
        }
        else {
            y = (int)((u32)y << 10) / x;
            index = y << 1;
        }
        angle = *(s16 *)((char *)atan_table + index);
    } else {
        if (x & 0x7FE00000) {
            y = x / (y >> 10);
            index = y << 1;
            asm volatile("" : : "r"(index));
        }
        else {
            y = (int)((u32)x << 10) / y;
            index = y << 1;
        }
        angle = *(s16 *)((char *)atan_table + index);
        asm volatile("" : "=r"(angle) : "0"(angle));
        angle = 1024 - angle;
    }
    if (negative_x) angle = 2048 - angle;
    if (negative_y) angle = -angle;
    result = angle;
done:
    return result;
}
