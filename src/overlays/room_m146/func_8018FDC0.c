typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

void func_8018FDC0(void *arg0, u8 *state, char *work) {
    int i;
    register char *cur asm("$4");
    unsigned int phase;
    int delta;

    *(u16 *)(work + 0x106) += *(u16 *)(work + 0x108);

    if (*(s16 *)(work + 0x102) >= 0x11) {
        *(s16 *)(work + 0x102) -= 0x10;
    }

    i = 0;
    cur = work;
    do {
        delta = (*(s16 *)(cur + 0x80) * *(s16 *)(work + 0x106)) >> 16;
        *(u16 *)(cur + 2) += 8;
        *(u16 *)(cur + 0) += delta;
        delta = (*(s16 *)(cur + 0x84) * *(s16 *)(work + 0x106)) >> 16;
        i++;
        *(u16 *)(cur + 4) += delta;
        cur += 8;
    } while ((unsigned int)i < 0x10);

    phase = work[0x105];
    phase += 6;
    work[0x105] = phase;
    phase &= 0xFF;
    phase >>= 4;
    work[0x104] = phase + 1;
    if (phase == 7) {
        work[0x104] = 0;
        state[1] = 2;
    }
}
