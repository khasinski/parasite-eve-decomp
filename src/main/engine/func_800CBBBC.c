#include "common.h"
int func_800CBBBC(void *arg0, u8 *state, u8 *anim) {
    s16 value = *(u16 *)(anim + 4) - 0x14;

    *(u16 *)(anim + 4) = value;
    if (value < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}
