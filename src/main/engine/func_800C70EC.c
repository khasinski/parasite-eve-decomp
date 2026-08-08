#include "common.h"
void func_800C70EC(char *data, int rDelta, int gDelta, int bDelta) {
    int frame;
    register int offset asm("$2");
    int count;
    int i;
    u8 *entry;
    register u8 *bluePtr asm("$9");

        i = 0;
        offset = *(u16 *)(data + 0x8);
        count = *(u16 *)(data + 0xA);
    entry = (u8 *)(data + offset);
    if (count > 0) {
        bluePtr = entry + 2;
        do {
            register s16 value asm("$3");
            int calc;
            int blueValue;
            register int blue asm("$8");

            value = entry[0];
            calc = value + rDelta;
            value = calc;
            if ((s16)calc >= 0x100) {
                value = 0xFF;
            }
            if (value < 0) {
                value = 0;
            }
            entry[0] = value;

            value = bluePtr[-1];
            calc = value + gDelta;
            value = calc;
            if ((s16)calc >= 0x100) {
                value = 0xFF;
            }
            if (value < 0) {
                value = 0;
            }

            blue = bluePtr[0];
            blueValue = blue + bDelta;
            blue = blueValue;
            asm volatile("" : "=r"(blueValue) : "0"(blueValue));
            bluePtr[-1] = value;
            if ((s16)blueValue >= 0x100) {
                blue = 0xFF;
            }
            if ((s16)blue < 0) {
                blue = 0;
            }
            i++;
            bluePtr[0] = blue;
            bluePtr += 4;
            entry += 4;
        } while (i < *(u16 *)(data + 0xA));
    }
    asm volatile("" : : "r"(&frame));
}
