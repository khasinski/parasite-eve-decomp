typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

int func_800CC92C(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6);
    anim[3] -= 2;
    *(u16 *)(anim + 8) = *(u16 *)(anim + 8) - 0xA;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0x1E;
    if (*(s8 *)(anim + 3) < 0x1E) {
        state[1] = 2;
    }
}
