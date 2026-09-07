typedef struct { short x; unsigned short y; short z; unsigned short speed; } Particle;
typedef struct { unsigned char r, g, b, unknown; } Color;
extern Color D_8018F1E4;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Particle *, void *, int, int, int, int, int, int, Color *);

int func_80195E10(int mode, Particle *particle) {
    Color color = D_8018F1E4;
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        else {
            unsigned short y = particle->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((volatile Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int sample = *(short *)((char *)D_800966EC + (((unsigned int)frame << 8) & 0x3F00));
        int size, kind, palette;
        register int specialKind asm("$3") = 4;
        unsigned short clut;
        kind = D_800F336C;
        size = sample * 2;
        asm("" : : "r"(size) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        func_800CEE20(particle, 0, size, size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4] >> 6, &color);
    }
    return 0;
}
