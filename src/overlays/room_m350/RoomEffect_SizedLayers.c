typedef struct {
    int reserved;
    short x, y, z, size;
} Particle;

typedef struct {
    signed int size : 16;
    signed int shade : 16;
} TrigEntry;

extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern short D_800966EE[];
extern int D_8019A3CC[], D_8019A3C8[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, int, int, int, void *);
extern void func_800D004C(void *, int, int, int, void *, int, int, void *, void *, int, int);

int func_8019404C(int event, Particle *particle)
{
    /* Match note: original purpose of unused sp+0x30..0x37 is unknown. */
    char frameGap[8];

    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int size = *(short *)((char *)D_800966EC +
            ((((unsigned int)D_800E27EC - 1) << 9) & 0x3E00)) * particle->size / 4096;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        unsigned short clut;

        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20(&particle->x, 0, size, size, D_800F336A * 5 + 216, clut, 1,
            ((TrigEntry *)((char *)D_800966EE - 2 +
                ((((unsigned int)D_800E27EC - 1) << 9) & 0x3E00)))->shade >> 5, particle);
        func_800D004C(&particle->x, 96, 96, 8, 0, size, size, D_8019A3CC, D_8019A3C8,
            (short)D_800966EC[(((unsigned int)D_800E27EC - 1) << 8) & 0xF00] >> 5, 1);
    }
    return 0;
}
