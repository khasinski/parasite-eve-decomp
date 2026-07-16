typedef unsigned char u8;
typedef unsigned short u16;

int func_800CCAB0(void *arg0, u8 *state, u8 *anim) {
    int i;
    register u8 *state_t1 asm("$9");
    register u8 *entry_a3 asm("$7");
    register int dx asm("$3");
    register int dy asm("$4");
    register int dz asm("$5");
    int frame;

    asm volatile("" : : "r"(&frame));
    state_t1 = state;
    asm volatile("" : "=r"(state_t1) : "0"(state_t1));
    i = 0;
    if (*(short *)(anim + 0x4) > 0) {
        entry_a3 = anim;
        do {
            register int pos asm("$2");
            register int angle asm("$3");

            i++;
            dx = *(short *)(entry_a3 + 0x68) >> 8;
            pos = *(u16 *)(entry_a3 + 0x8);
            dy = *(short *)(entry_a3 + 0x88) >> 8;
            dz = *(short *)(entry_a3 + 0xA8) >> 8;
            pos += dx;
            *(u16 *)(entry_a3 + 0x8) = pos;
            pos = *(u16 *)(entry_a3 + 0x28);
            angle = *(u16 *)(entry_a3 + 0x88);
            pos += dy;
            *(u16 *)(entry_a3 + 0x28) = pos;
            pos = *(u16 *)(entry_a3 + 0x48);
            angle += 0xB4;
            *(u16 *)(entry_a3 + 0x88) = angle;
            pos += dz;
            *(u16 *)(entry_a3 + 0x48) = pos;
            entry_a3 += 2;
        } while (i < *(short *)(anim + 0x4));
    }

    anim[3] -= 2;
    if ((signed char)anim[3] < 2) {
        state_t1[1] = 2;
    }
    asm volatile("" : : "r"(&frame));
}
