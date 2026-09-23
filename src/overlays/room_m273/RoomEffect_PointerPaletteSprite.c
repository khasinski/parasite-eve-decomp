typedef struct { int x, y, z; } Vec3;
typedef struct { short x, y, z, pad; } Sprite;
typedef struct { Vec3 *position; } EffectState;
#include "room_m273.h"
#include "pe1/psyq_gpu.h"

extern char D_8019AB68[];

int func_80193870(int mode, EffectState *state) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int index = ((D_800E27EC - 1) << 8) & 0x3f00;
        Sprite sprite;
        int product;
        int sample;
        short size;
        short shade;
        volatile Vec3 *p = state->position;
        int kind;
        /* Keep the comparison constant in v1, as in the retail callback. */
        register int special asm("$3");
        int palette;
        int x;

        product = ((short *)D_800966EC)[index / 2] * 2 + 4096;
        size = product;
        sample = ((short *)D_800966EC)[index / 2 + 1];
        shade = (unsigned int)sample >> 5;
        x = p->x;
        sprite.y = p->y;
        sprite.z = p->z;
        special = 4;
        sprite.x = x;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) palette += 9;
        else palette += 5;
        func_800CEE20(&sprite, D_8019AB68, (short)size, (short)size, 102,
                        GetClut(0,palette), 1, (short)shade, 0);
    }
    return 0;
}
