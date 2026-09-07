typedef struct { short x, y, z, pad; } Vector;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);

int func_80194470(int mode, Vector *input) {
    Vector *position = input;
    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        int firstFrame = D_800E27EC - 1;
        int frame;
        unsigned int offset;
        int kind = D_800F336C;
        int size, palette;
        register int specialKind asm("$3");
        unsigned short clut;
        asm("" : : "r"(firstFrame), "r"(position));
        offset = ((unsigned int)firstFrame << 9) & 0x3E00;
        frame = firstFrame;
        size = *(short *)((char *)D_800966EC + offset) * 2 + 4096;
        specialKind = 4;
        /* Preserve the size capture and saved frame before palette lookup. */
        asm("" : : "r"(size), "r"(frame) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 6, 0);
    }
    return 0;
}
