typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int func_800CBB30(void *arg0, u8 *state, u8 *anim) {
    u8 *cur = anim;

    *(u16 *)(anim + 8) = *(u16 *)(anim + 8) + *(u16 *)(anim + 0x10);
    *(u16 *)(anim + 0xA) = *(u16 *)(anim + 0xA) + *(u16 *)(anim + 0x12);
    *(u16 *)(anim + 0xC) = *(u16 *)(anim + 0xC) + *(u16 *)(anim + 0x14);
    *(u16 *)(anim + 0x12) = *(u16 *)(anim + 0x12) + 3;
    anim[1]++;
    if (*(s16 *)(anim + 0xA) > 0) {
        *(u16 *)(anim + 0x12) = -*(u16 *)(anim + 0x12);
    }
    if (cur[2]-- == 0) {
        state[1] = 2;
    }
}
