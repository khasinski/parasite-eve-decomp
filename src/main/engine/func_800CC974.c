typedef unsigned char u8;
typedef unsigned short u16;

int func_800CC974(void *arg0, u8 *state, u8 *anim) {
    int i;
    register u8 *state_t2 asm("$10");
    register u8 *timer_t0 asm("$8");
    register u8 *entry_a3 asm("$7");
    register int dx asm("$3");
    register int dy asm("$4");
    register int dz asm("$5");
    int frame;

    asm volatile("" : : "r"(&frame));
    state_t2 = state;
    asm volatile("" : "=r"(state_t2) : "0"(state_t2));
    i = 0;
    if ((signed char)anim[2] > 0) {
        timer_t0 = anim;
        asm volatile("" : "=r"(timer_t0) : "0"(timer_t0));
        entry_a3 = anim;
        do {
            register int pos asm("$2");
            register int angle asm("$3");

            i++;
            dx = *(short *)(entry_a3 + 0xA6) >> 9;
            pos = *(u16 *)(entry_a3 + 0x26);
            dy = *(short *)(entry_a3 + 0xA8) >> 9;
            dz = *(short *)(entry_a3 + 0xAA) >> 9;
            pos += dx;
            *(u16 *)(entry_a3 + 0x26) = pos;
            pos = *(u16 *)(entry_a3 + 0x28);
            angle = *(u16 *)(entry_a3 + 0xA8);
            pos += dy;
            *(u16 *)(entry_a3 + 0x28) = pos;
            pos = *(u16 *)(entry_a3 + 0x2A);
            angle += 0xB4;
            *(u16 *)(entry_a3 + 0xA8) = angle;
            pos += dz;
            *(u16 *)(entry_a3 + 0x2A) = pos;
            pos = *(u16 *)(timer_t0 + 0x6);
            entry_a3 += 8;
            pos += 0x18;
            *(u16 *)(timer_t0 + 0x6) = pos;
            timer_t0 += 2;
        } while (i < (signed char)anim[2]);
    }

    anim[1] -= 2;
    if (anim[1] < 2) {
        state_t2[1] = 2;
    }
    asm volatile("" : : "r"(&frame));
}
