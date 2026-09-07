typedef struct { short unit, scale; } RenderState;
extern volatile int D_800E27EC;
extern volatile RenderState D_800F3368;
extern volatile short D_800F336A, D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800F336C;
extern unsigned short D_800F3370;
extern volatile short D_800F3376, D_800F3378;
extern volatile unsigned short D_800E11E8, D_800E1208;
extern unsigned short D_800E2850[];
extern short D_800966EC[], D_800966EE[];
extern int D_8019A59C[];
/* Signed high-half view avoids LHU followed by explicit sign extension. */
typedef struct { signed int unused : 16; signed int value : 16; } SignedHalf;
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80196C64(int event, void *object)
{
    /* Match note: retail leaves sp+0x28..0x2F unused; original local unknown. */
    char frameGap[8];
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        volatile RenderState *state = &D_800F3368;
        int unit = 32;
        register int size asm("$16");
        int index;
        int palette;
        int frame;
        register int mode asm("$5");
        register int clutX asm("$4");
        unsigned short handle;
        asm("" : "=r"(state) : "0"(state));
        state->unit = unit;
        index = D_800E11E8;
        mode = 2;
        D_800F336A = mode;
        D_800F3376 = unit;
        D_800F3378 = unit;
        D_800F3376 = unit;
        D_800F3378 = unit;
        palette = D_800E2850[index];
        /* Capture the palette before reading the animation frame. */
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        frame = D_800E27EC;
        clutX = 0;
        D_800F336C = mode;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = unit;
        index = ((unsigned int)(frame - 1) << 9) & 0x3E00;
        asm("" : "=r"(index) : "0"(index) : "memory");
        D_800F3370 = palette;
        size = *(short *)((char *)D_800966EC + index);
        asm volatile("" : : : "memory");
        size = size * 2 + 4096;
        handle = GetClut(clutX, D_800E1208);
        {
            register int frame asm("$6") = D_800E27EC - 1;
            int phase = (frame >> 2) & 3;
            register int *colors asm("$3");
            frame = ((unsigned int)frame << 9) & 0x3E00;
            colors = D_8019A59C;
            func_800CEE20(object, 0, (short)size, (short)size,
                ((RenderState *)state)->scale * phase + 192,
                handle, 1, ((SignedHalf *)((char *)D_800966EE - 2 +
                    frame))->value >> 5, colors);
        }
    }
    return 0;
}
