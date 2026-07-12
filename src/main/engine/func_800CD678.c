typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

int func_800CD678(void *arg0, u8 *state, u8 *anim) {
    register int value asm("$2") = rand() % 11;
    u16 z = *(u16 *)(anim + 0xA);
    register u16 x asm("$5") = *(u16 *)(anim + 0x4);
    register u16 y asm("$6") = *(u16 *)(anim + 0x8);

    z -= 8;
    x -= 8;
    y -= 5;
    asm("" : "=r"(y) : "0"(y));

    *(u16 *)(anim + 0xA) = z;
    *(u16 *)(anim + 0x4) = x;
    y += value;
    *(u16 *)(anim + 0x8) = y;

    if ((short)x < 8) {
        state[1] = 2;
    }
}
