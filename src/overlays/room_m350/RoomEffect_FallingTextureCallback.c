extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern short D_800966EE[];
extern int D_8019A464[];
typedef struct { signed int unused:16; signed int value:16; } SignedHalf;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_801937B4(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3]++;
    } else if (event == 2) {
        int kind = D_800F336C;
        register int specialKind asm("$4");
        int texture;
        int handle;
        asm("" : "=r"(kind) : "0"(kind));
        specialKind = 4;
        texture = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) texture += 6;
        else texture += 2;
        handle = GetClut(0, texture);
        func_800CEE20(position, 0, 8192, 8192,
            D_800F336A * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            ((SignedHalf *)((char *)D_800966EE - 2 +
              (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)))->value >> 5,
            D_8019A464);
    }
    return 0;
}
