#include "common.h"
#include "pe1/gte_types.h"

typedef struct SceneE19Effect {
    u16 x, y, z, pad;
    s16 state;
    s16 frame;
} SceneE19Effect;

extern GteShortVector D_8018F1CC;
extern int D_800F3428, D_8019B668;
extern u16 D_800F336C, D_800E1204[];
extern int func_80077CF4(int), func_80077DC4(int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_80192E08(int mode, SceneE19Effect *effect) {
    GteShortVector position = D_8018F1CC;
    int angle, cosine, sine, brightness, palette, kind;
    u16 clut;

    switch (mode) {
    case 1: {
        if (effect->state != 0) return 0;
        effect->frame++;
        effect->y -= 128;
        if (effect->frame >= 8) return 1;
        break;
    }
    case 2: {
        if (effect->state != 0) return 0;
        position.z = 1024;
        angle = effect->frame << 7;
        cosine = func_80077CF4(angle);
        sine = func_80077DC4(angle);
        brightness = (D_8019B668 * sine) / 4096;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(144, palette);
        func_800CEE20(effect, &position, 4096, cosine, 0x88, clut, 1, brightness, 0);
        break;
    }
    }
    return 0;
}
