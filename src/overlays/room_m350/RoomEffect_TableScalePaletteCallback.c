extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int D_800966EC[];
extern unsigned char D_8019A4EC[];
extern int D_8019A3D0[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80195564(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int kind = D_800F336C;
        register int specialKind asm("$4");
        int texture;
        int handle;
        asm("" : "=r"(kind) : "0"(kind));
        specialKind = 4;
        texture = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) texture += 8;
        else texture += 4;
        handle = GetClut(0, texture);
        func_800CEE20(position, 0, position[3], position[3],
            D_800F336A * D_8019A4EC[(D_800E27EC - 1) / 2] + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A3D0);
    }
    return 0;
}
