#include "common.h"
int func_800CD5B0(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 0xA;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x3C;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}
