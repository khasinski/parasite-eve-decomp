#include "common.h"

extern int func_80071A54(void);

void func_80192F50(void *unused, char *condition_arg, char *effect) {
    /* Keep the four live cursors in the registers used by the original loop. */
    register char *condition asm("$19") = condition_arg;
    register char *base asm("$18") = effect;
    register char *state asm("$16") = base;
    register char *data asm("$17") = base;
    int random;

    do {
        if (*(u8 *)state == 0) {
            if (func_80071A54() % 30 == 0 && *(s16 *)(condition + 2) < 50) {
                *(u8 *)state = 1;
                *(u8 *)(state + 0x20) = 0;
                *(s16 *)(data + 0x40) = func_80071A54() % 4096;
                *(s16 *)(data + 0x80) = 0x7e8;
                *(s16 *)(data + 0xc0) = 0x7e8;
                random = func_80071A54();
                *(u8 *)(state + 0x100) = 0;
                *(u8 *)(state + 0x120) = random % 20 + 40;
            }
        } else {
            *(u8 *)(state + 0x20) += 1;
            *(u16 *)(data + 0x40) += *(u8 *)(state + 0x120);
            *(u16 *)(data + 0xc0) -= *(u8 *)(state + 0x100);
            *(u8 *)(state + 0x100) += 2;
            *(u16 *)(data + 0x80) -= 40;
            if (*(s16 *)(data + 0x80) < 0) {
                *(s16 *)(data + 0x80) = 0;
            }
            if (*(s16 *)(data + 0xc0) < 0) {
                *(u8 *)state = 0;
            }
        }
        state++;
        data += 2;
    } while (state < base + 32);

    if (*(s16 *)(condition + 2) >= 121) {
        *(u8 *)(condition + 1) = 2;
    }
}
