typedef struct { short x; unsigned short y; short z; unsigned short speed; } Particle;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern short D_800F336A;
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned char D_8019AD70[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Particle *, void *, int, int, int, int, int, int, void *);

int func_801974DC(int mode, Particle *input) {
    Particle *particle = input;
    /* Layout only: original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 32) return 1;
        else {
            unsigned short y = ((volatile Particle *)particle)->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((volatile Particle *)particle)->speed;
            speed += 1;
            asm("" : "=r"(y) : "0"(y), "r"(speed));
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F336C;
        int sample, palette, size, shade;
        int paletteOffset, sizeOffset;
        unsigned int sampleOffset;
        int clut;
        sampleOffset = ((unsigned int)frame << 8) & 0x3F00;
        asm("" : : "r"(sampleOffset), "r"(frame));
        paletteOffset = kind * 2;
        sample = D_800966EC[sampleOffset / 4];
        asm("" : : "r"(sample), "r"(paletteOffset));
        sizeOffset = ((unsigned int)frame << 7) & 0x3F80;
        palette = *(unsigned short *)((char *)D_800E1204 + paletteOffset);
        shade = (short)sample >> 5;
        size = *(short *)((char *)D_800966EC + sizeOffset) + 8192;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(particle, 0, size, size, D_800F336A * (frame / 4) + 200,
            clut, 1, shade, D_8019AD70);
    }
    return 0;
}
