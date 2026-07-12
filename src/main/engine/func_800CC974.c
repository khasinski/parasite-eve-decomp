typedef unsigned char u8;
typedef unsigned short u16;

int func_800CC974(void *arg0, u8 *state, u8 *anim) {
    int i;
    u8 *timer_entry;
    u8 *entry;

    if ((signed char)anim[2] > 0) {
        i = 0;
        timer_entry = anim;
        entry = anim;
        do {
            i++;
            *(u16 *)(entry + 0x26) = *(u16 *)(entry + 0x26) + (*(short *)(entry + 0xA6) >> 9);
            *(u16 *)(entry + 0x28) = *(u16 *)(entry + 0x28) + (*(short *)(entry + 0xA8) >> 9);
            *(u16 *)(entry + 0xA8) = *(u16 *)(entry + 0xA8) + 0xB4;
            *(u16 *)(entry + 0x2A) = *(u16 *)(entry + 0x2A) + (*(short *)(entry + 0xAA) >> 9);
            *(u16 *)(timer_entry + 0x6) = *(u16 *)(timer_entry + 0x6) + 0x18;
            entry += 8;
            if (i < (signed char)anim[2]) {
                timer_entry += 2;
            }
        } while (i < (signed char)anim[2]);
    }

    anim[1] -= 2;
    if (anim[1] < 2) {
        state[1] = 2;
    }
}
