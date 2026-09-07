typedef struct { void *position; } Effect;
extern int D_800E27EC, D_800F3428;
extern int D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_80199950(int mode, Effect *effect) {
    /* Layout only: the original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int frame = (short)(D_800E27EC - 1);
        int kind = D_800F336C;
        unsigned int sizeOffset, shadeOffset;
        int size, shade, palette;
        register int specialKind asm("$3");
        unsigned short clut;
        sizeOffset = ((unsigned int)frame << 8) & 0x3F00;
        shadeOffset = ((unsigned int)frame << 9) & 0x3E00;
        size = *(int *)((char *)D_800966EC + sizeOffset) + 2048;
        shade = (short)*(int *)((char *)D_800966EC + shadeOffset) >> 6;
        specialKind = 4;
        asm("" : "=r"(size), "=r"(shade) : "0"(size), "1"(shade) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 7;
        else palette += 3;
        clut = GetClut(0, palette);
        func_800CEE20(effect->position, 0, (short)size, (short)size,
            64, clut, 1, (short)shade, 0);
    }
    return 0;
}
