#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"

extern int D_800966EC[];
extern short D_800F336A;
extern GteRotation D_801956D8;
extern RenderColor D_801956E0;
int func_801946D0(int mode, GteShortVector *effect) {
    int texture, size, kind, palette;
    register int frame asm("$3");
    int frameLater;
    u16 clut;
    if (mode == 1) goto check_lifetime;
    goto render;
check_lifetime:
    if (D_800E27EC < 9) goto finished;
    goto expired;
render:
    if (mode != 2) return 0;
    frame = D_800E27EC - 1;
    kind = D_800F336C;
    size = D_800966EC[((unsigned int)frame << 9 & 0x3E00) / 4] + 0x200;
    palette = D_800E1204[kind];
    frameLater = frame;
    if (kind == 4 && D_800F3428) palette += 4;
    clut = GetClut(0x80, palette);
    texture = D_800F336A * 2 + 0xFD;
    /* The signed high halfword is the intensity sample for this frame. */
    func_800CEE20(effect, &D_801956D8, (s16)size, (s16)size,
                   texture, clut, 1,
                   ((s16)(*(s32 *)((u8 *)D_800966EC +
                       (((u32)frameLater << 9) & 0x3E00)) >> 16)) >> 5,
                   &D_801956E0);
finished:
    return 0;
expired:
    return 1;
}
