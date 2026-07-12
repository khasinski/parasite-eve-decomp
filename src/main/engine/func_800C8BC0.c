typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int func_800C8BC0(void *arg0, u8 *state, u8 *anim) {
    u8 count;
    u16 zSpeed;

    *(u16 *)(anim + 0x8) += *(u16 *)(anim + 0x10);
    *(u16 *)(anim + 0xA) += *(u16 *)(anim + 0x12);
    *(u16 *)(anim + 0xC) += *(u16 *)(anim + 0x14);

    zSpeed = *(u16 *)(anim + 0x12) + 3;
    *(u16 *)(anim + 0x12) = zSpeed;

    count = anim[2];
    anim[1]++;
    if (*(s16 *)(anim + 0xA) > 0) {
        *(u16 *)(anim + 0x12) = -zSpeed;
    }

    anim[2] = count - 1;
    if (count == 0) {
        state[1] = 2;
    }
}
