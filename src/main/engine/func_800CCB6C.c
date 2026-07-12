typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

int func_800CCB6C(void *arg0, u8 *state, u8 *anim) {
    anim[3] -= 8;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0x1A4;
    if (*(s8 *)(anim + 3) < 0x14) {
        anim[3] = 0;
        state[1] = 2;
    }
}
