#include "common.h"
int func_800CC92C(void *arg0, u8 *state, u8 *anim) {
    u16 value;
    register int frame asm("$3");

    value = *(u16 *)(anim + 6);
    frame = anim[3];
    *(u16 *)(anim + 6) = value;
    value = *(u16 *)(anim + 8);
    frame -= 2;
    anim[3] = frame;
    asm("" ::: "memory");
    frame = *(signed char *)(anim + 3);
    value -= 0xA;
    *(u16 *)(anim + 8) = value;
    value = *(u16 *)(anim + 4);
    frame = frame < 0x1E;
    value += 0x1E;
    *(u16 *)(anim + 4) = value;
    if (frame) {
        state[1] = 2;
    }
}
