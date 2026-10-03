#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"

typedef struct {
    GteShortVector position;
    u16 base;
} SceneE18PaletteEffect;

extern s32 D_800966EC[];
extern s16 D_800F336A;

int func_80193488(int mode, SceneE18PaletteEffect *effect) {
    int size, kind, palette, texture, base;
    int firstFrame, secondFrame;
    int sample;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        effect->position.y -= effect->position.pad;
        effect->position.pad++;
    } else if (mode == 2) {
        firstFrame = D_800E27EC - 1;
        sample = D_800966EC[(((unsigned int)firstFrame << 8) & 0x3F00) / 4];
        base = effect->base;

        kind = D_800F336C;
        size = base + sample;
        palette = D_800E1204[kind];

        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2);
        secondFrame = D_800E27EC - 1;
        texture = D_800F336A * ((secondFrame >> 1) & 7);
        func_800CEE20(&effect->position, 0, (s16)size, (s16)size,
                       texture, clut, 1,
                       (s16)D_800966EC[(((unsigned int)secondFrame << 9) & 0x3E00) / 4] >> 5,
                       0);
    }
    return 0;
}
