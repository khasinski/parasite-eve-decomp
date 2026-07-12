typedef unsigned char u8;
typedef unsigned short u16;

int func_800CD5EC(void *arg0, u8 *state, u8 *anim) {
    unsigned int i = 0;

    do {
        u8 *entry = anim + ((i & 0xFFFF) * 8);

        i++;
        *(u16 *)(entry + 0x10) = *(u16 *)(entry + 0x10) + *(u16 *)(entry + 0x20);
        *(u16 *)(entry + 0x12) = *(u16 *)(entry + 0x12) + *(u16 *)(entry + 0x22);
        *(u16 *)(entry + 0x14) = *(u16 *)(entry + 0x14) + *(u16 *)(entry + 0x24);
    } while ((i & 0xFFFF) < 2);

    anim[3]++;
    if ((signed char)anim[3] >= 8) {
        state[1] = 2;
    }
}
