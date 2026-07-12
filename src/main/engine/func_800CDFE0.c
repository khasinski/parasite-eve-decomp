typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

int func_800CDFE0(void *arg0, u8 *state, u8 *anim) {
    int value = rand();
    u16 z = *(u16 *)(anim + 0x6);
    u8 counter = anim[1];
    u16 x = *(u16 *)(anim + 0x4);

    z -= 4;
    counter -= 3;
    x -= 5;

    *(u16 *)(anim + 0x6) = z;
    anim[1] = counter;

    if ((signed char)counter < 3) {
        *(u16 *)(anim + 0x4) = x + (value % 11);
        state[1] = 2;
    } else {
        *(u16 *)(anim + 0x4) = x + (value % 11);
    }
}
