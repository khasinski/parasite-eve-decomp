typedef struct { short x; unsigned short y; short z; unsigned short speed; } Particle;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Particle *, void *, int, int, int, int, int, int, void *);

int func_80199568(int mode, Particle *particle) {
    if (mode == 1) {
        int frame = D_800E27EC;
        if (frame >= 36) return 1;
        if (frame >= 4) {
            unsigned short y = ((volatile Particle *)particle)->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((volatile Particle *)particle)->speed;
            speed += 1;
            asm("" : "=r"(y) : "0"(y), "r"(speed));
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 5;
        int kind, palette, clut;
        if (frame < 0) return 0;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(particle, 0, 12288, 12288,
            D_800F336A * (frame / 4) + 200, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / 4] >> 5, 0);
    }
    return 0;
}
