typedef struct { short x, y, z, pad; } Vector;
extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A, D_8019AD68[];
extern int D_800966EC[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);

int func_80196F2C(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        position->y -= 6;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int clut;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(position, 0, 4096, 4096,
            44 + D_800F336A * D_8019AD68[frame / 4], clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4] >> 5, 0);
    }
    return 0;
}
