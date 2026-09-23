#include "room_m273.h"
#include "pe1/psyq_gpu.h"

extern s16 D_800F336A;
extern char D_8019AE00[];

int func_80198060(int mode, GteShortVector *position) {
    int frame, sample, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC;
        if (frame - 2 < 0) return 0;

        sample = *(s16 *)((char *)D_800966EC + (((frame - 1) << 9) & 0x3E00) + 2);
        size = sample * 2;
        kind = D_800F336C;
        /* Keep the table read before the palette lookup. */
        asm volatile("" ::: "memory");
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;

        clut = GetClut(32, palette);
        func_800CEE20(position, 0, (s16)size, (s16)size,
                       D_800F336A + 216, clut, 1,
                       /* The upper half of each packed trig entry is signed. */
                       (s16)(*(s32 *)((char *)D_800966EC +
                           (((D_800E27EC - 1) << 9) & 0x3E00)) >> 16) >> 5,
                       (RenderColor *)D_8019AE00);
    }
    return 0;
}
