typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

int func_800CC92C(void *arg0, u8 *state, u8 *anim) {
    register u16 value asm("$2");
    register int frame asm("$3");

    value = *(u16 *)(anim + 6);
    frame = anim[3];
    *(u16 *)(anim + 6) = value;
    value = *(u16 *)(anim + 8);
    frame -= 2;
    anim[3] = frame;
    asm volatile("lb $3,3(%1)" : "=r"(frame) : "r"(anim));
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
