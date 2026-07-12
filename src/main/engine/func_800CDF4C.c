typedef unsigned char u8;
typedef unsigned short u16;

int func_800CDF4C(void *arg0, u8 *state, u8 *anim) {
    unsigned int i = 0;

    anim[1] -= 0x10;
    anim[3] += 0xC;

    do {
        u8 *entry = anim + ((i & 0xFFFF) * 2);

        i++;
        *(u16 *)(entry + 0x10) = *(u16 *)(entry + 0x10) + *(u16 *)(entry + 0x40);
        *(u16 *)(entry + 0x20) = *(u16 *)(entry + 0x20) + *(u16 *)(entry + 0x50);
        *(u16 *)(entry + 0x30) = *(u16 *)(entry + 0x30) + *(u16 *)(entry + 0x60);
    } while ((i & 0xFFFF) < 8);

    if ((signed char)anim[1] < 0x10) {
        state[1] = 2;
    }
}
