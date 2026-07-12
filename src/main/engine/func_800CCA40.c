typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

int func_800CCA40(void *arg0, u8 *state, u8 *anim) {
    anim[3] -= 2;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0x28;
    if (*(s8 *)(anim + 3) < 0x1E) {
        state[1] = 2;
    }
}
