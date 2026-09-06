extern int D_800E27EC;
extern short D_800966EC[];
typedef struct { signed int scale:16; signed int shade:16; } EffectEntry;
extern int D_8019A3C8[], D_8019A628[];
extern void func_800D004C(void *, int, int, int, short *, int, int,
                         int *, int *, int, int);

int func_8019A21C(int event, void *context) {
    short rotation[4];
    short *entry;
    int scale;
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int angle = (D_800E27EC << 12) / 20;
        unsigned int offset = (unsigned int)(D_800E27EC - 1) << 8;
        short *base = D_800966EC;
        entry = (short *)((offset & 0x3F00) + (char *)base);
        scale = entry[0];
        rotation[0] = 1024;
        rotation[1] = 0;
        rotation[2] = angle;
        rotation[3] = 1;
        func_800D004C(context, 384, 384, 10, rotation, scale, scale,
                     D_8019A3C8, D_8019A628,
                     ((EffectEntry *)entry)->shade >> 5, 1);
    }
    return 0;
}
