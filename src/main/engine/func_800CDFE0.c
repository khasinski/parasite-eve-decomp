typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

int func_800CDFE0(void *arg0, u8 *state, u8 *anim) {
    register int value asm("$2") = rand() % 11;
    u16 z = *(u16 *)(anim + 0x6);
    register u8 counter asm("$5") = anim[1];
    register u16 x asm("$6") = *(u16 *)(anim + 0x4);

    z -= 4;
    counter -= 3;
    x -= 5;
    asm("" : "=r"(x) : "0"(x));

    *(u16 *)(anim + 0x6) = z;
    anim[1] = counter;
    x += value;
    *(u16 *)(anim + 0x4) = x;

    if ((signed char)counter < 3) {
        state[1] = 2;
    }
}
