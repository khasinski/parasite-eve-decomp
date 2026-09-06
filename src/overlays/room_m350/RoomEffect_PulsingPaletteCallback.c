extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800966EE[];
extern int D_800966EC[];
typedef struct { int reserved[2]; void *position; int colors[1]; } Effect;
extern int GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);

int func_801927A4(int event, Effect *effect) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int scale = D_800966EE[((unsigned int)(D_800E27EC - 1) << 8) & 0x1F00] * 4 + 2048;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;
        if (kind == 4 && D_800F3428) palette += 4;
        handle = GetClut(48, palette);
        func_800CEE20(effect->position, effect, scale, scale, 110,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            effect->colors);
    }
    return 0;
}
