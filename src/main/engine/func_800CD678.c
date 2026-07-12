typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

int func_800CD678(void *arg0, u8 *state, u8 *anim) {
    int value = rand();
    u16 z = *(u16 *)(anim + 0xA);
    u16 x = *(u16 *)(anim + 0x4);
    u16 y = *(u16 *)(anim + 0x8);

    z -= 8;
    x -= 8;
    y -= 5;

    *(u16 *)(anim + 0xA) = z;
    *(u16 *)(anim + 0x4) = x;

    if ((short)x < 8) {
        *(u16 *)(anim + 0x8) = y + (value % 11);
        state[1] = 2;
    } else {
        *(u16 *)(anim + 0x8) = y + (value % 11);
    }
}
