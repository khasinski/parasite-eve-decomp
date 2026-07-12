typedef unsigned char u8;
typedef unsigned short u16;

int func_800CCAB0(void *arg0, u8 *state, u8 *anim) {
    int i;
    register u8 *state_t1 asm("$9");
    register u8 *entry_a3 asm("$7");
    int dx;
    int dy;
    register int dz asm("$5");
    int frame;

    asm volatile("" : : "r"(&frame));
    state_t1 = state;
    asm volatile("" : "=r"(state_t1) : "0"(state_t1));
    i = 0;
    if (*(short *)(anim + 0x4) > 0) {
        entry_a3 = anim;
        do {
            i++;
            dx = *(short *)(entry_a3 + 0x68) >> 8;
            dy = *(short *)(entry_a3 + 0x88) >> 8;
            dz = *(short *)(entry_a3 + 0xA8) >> 8;
            *(u16 *)(entry_a3 + 0x8) = *(u16 *)(entry_a3 + 0x8) + dx;
            *(u16 *)(entry_a3 + 0x28) = *(u16 *)(entry_a3 + 0x28) + dy;
            *(u16 *)(entry_a3 + 0x88) = *(u16 *)(entry_a3 + 0x88) + 0xB4;
            *(u16 *)(entry_a3 + 0x48) = *(u16 *)(entry_a3 + 0x48) + dz;
            entry_a3 += 2;
        } while (i < *(short *)(anim + 0x4));
    }

    anim[3] -= 2;
    if ((signed char)anim[3] < 2) {
        state_t1[1] = 2;
    }
    asm volatile("" : : "r"(&frame));
}
