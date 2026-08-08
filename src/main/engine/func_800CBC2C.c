#include "common.h"
int func_800CBC2C(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 8;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x78;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}
