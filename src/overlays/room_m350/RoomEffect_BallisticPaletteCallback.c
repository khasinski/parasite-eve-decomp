extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int D_800966EC[], D_8019A598[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_801968D8(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] += position[3];
        position[3] -= position[4];
    } else if (event == 2) {
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;
        if (kind == 4 && D_800F3428) palette += 4;
        handle = GetClut(16, palette);
        func_800CEE20(position, 0, 12288, 12288,
            D_800F336A * (((D_800E27EC - 1) >> 1) & 7) + 200,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A598);
    }
    return 0;
}
