typedef unsigned char u8;
typedef unsigned short u16;

int func_800CCAB0(void *arg0, u8 *state, u8 *anim) {
    int i;
    u8 *entry;

    if (*(short *)(anim + 0x4) > 0) {
        i = 0;
        entry = anim;
        do {
            i++;
            *(u16 *)(entry + 0x8) = *(u16 *)(entry + 0x8) + (*(short *)(entry + 0x68) >> 8);
            *(u16 *)(entry + 0x28) = *(u16 *)(entry + 0x28) + (*(short *)(entry + 0x88) >> 8);
            *(u16 *)(entry + 0x88) = *(u16 *)(entry + 0x88) + 0xB4;
            *(u16 *)(entry + 0x48) = *(u16 *)(entry + 0x48) + (*(short *)(entry + 0xA8) >> 8);
            if (i < *(short *)(anim + 0x4)) {
                entry += 2;
            }
        } while (i < *(short *)(anim + 0x4));
    }

    anim[3] -= 2;
    if ((signed char)anim[3] < 2) {
        state[1] = 2;
    }
}
