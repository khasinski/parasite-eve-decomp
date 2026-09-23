#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"

typedef struct {
    GteShortVector position;
    u16 base;
} SceneE18PaletteEffect;

extern s32 D_800966EC[];
extern s16 D_800F336A;

int func_80193488(int mode, SceneE18PaletteEffect *effect) {
    int size, kind, palette, texture, base, index;
    int firstFrame, secondFrame;
    register int sample asm("$3");
    register int special asm("$3");
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        effect->position.y -= effect->position.pad;
        effect->position.pad++;
    } else if (mode == 2) {
        firstFrame = D_800E27EC - 1;
        sample = *(s32 *)((char *)D_800966EC +
            (((unsigned int)firstFrame << 8) & 0x3F00));
        /* Preserve the sample read before the effect's base value. */
        asm volatile("" ::: "memory");
        base = effect->base;

        kind = D_800F336C;
        size = base + sample;
        index = kind * 2;
        asm volatile("" : : "r"(index));
        special = 4;
        palette = *(u16 *)((char *)D_800E1204 + index);
        if (kind == special && D_800F3428) palette += 6;
        else palette += 2;

        clut = GetClut(0, palette);
        secondFrame = D_800E27EC - 1;
        texture = D_800F336A * ((secondFrame >> 1) & 7);
        func_800CEE20(&effect->position, 0, (s16)size, (s16)size,
                       texture, clut, 1,
                       (s16)D_800966EC[(((unsigned int)secondFrame << 9) & 0x3E00) / 4] >> 5,
                       0);
    }
    return 0;
}
