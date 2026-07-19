typedef struct RoomM153Particle {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short unk6;
} RoomM153Particle;

void func_8018FF68(void *arg0, unsigned char *signal, char *rec) {
    unsigned int i;
    RoomM153Particle *cur;
    unsigned char phase;
    int delta;

    *(unsigned short *)(rec + 0x106) += *(unsigned short *)(rec + 0x108);
    if (*(short *)(rec + 0x102) >= 0x11) {
        *(unsigned short *)(rec + 0x102) -= 0x10;
    }

    for (i = 0; i < 0x10; i++) {
        cur = (RoomM153Particle *)(rec + (i * sizeof(RoomM153Particle)));
        delta = (*(short *)((char *)cur + 0x80) * *(short *)(rec + 0x106)) >> 16;
        cur->y += 8;
        cur->x += delta;
        delta = (*(short *)((char *)cur + 0x84) * *(short *)(rec + 0x106)) >> 16;
        cur->z += delta;
    }

    phase = *(unsigned char *)(rec + 0x105) + 6;
    *(unsigned char *)(rec + 0x105) = phase;
    *(unsigned char *)(rec + 0x104) = (phase >> 4) + 1;
    if ((phase >> 4) == 7) {
        *(unsigned char *)(rec + 0x104) = 0;
        signal[1] = 2;
    }
}
