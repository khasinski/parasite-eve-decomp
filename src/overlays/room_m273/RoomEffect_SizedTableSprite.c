typedef struct { short x, y, z, pad; } Vector;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);

int func_80197230(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        register int specialKind asm("$3") = 4;
        int kind, palette;
        register int size asm("$18");
        register int sample asm("$2");
        unsigned short clut;
        sample = D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4];
        /* Keep the sample read before the renderer-state read. */
        asm("" : : "r"(sample), "r"(specialKind) : "memory");
        kind = D_800F336C;
        size = sample + 2048;
        asm("" : : "r"(size));
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 5, 0);
    }
    return 0;
}
