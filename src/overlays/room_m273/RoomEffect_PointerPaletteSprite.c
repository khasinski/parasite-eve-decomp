#include "room_m273_effects.h"
#include "pe1/psyq_gpu.h"

typedef struct { short x, y, z, pad; } Sprite;

extern unsigned short D_800966EC[];
extern char D_8019AB68[];
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
extern RoomM273PlayerActorView *g_PlayerEntity;
extern unsigned char D_8019AE5C;
extern unsigned short D_800E11FA, D_800E2850[];
extern unsigned short D_800F3368, D_800F336A, D_800F336E;
extern unsigned short D_800F3370, D_800F3372, D_800F3374;
extern volatile unsigned short D_800F3376, D_800F3378;
extern int func_800CE560(void *, int, int, int (*)());
extern RoomM273SpritePoolEffect *func_800CE610(void *);

int func_80193870(int mode, RoomM273SpritePoolEffect *state) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int index = ((D_800E27EC - 1) << 8) & 0x3f00;
        Sprite sprite;
        int product;
        int sample;
        short size;
        short shade;
        volatile RoomM273WorldPosition *p = state->position;
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

int func_801939B4(int mode) {
    switch (mode) {
    case 0:
        D_8019AE5C = 0;
        return func_800CE560(D_800F33E0->pool, 4, 1, func_80193870);
    case 1:
        if (D_800E27EC >= 20) return 2;
        if (D_8019AE5C) return 2;
        if (g_PlayerEntity->kind < 4) {
            RoomM273SpritePoolEffect *effect =
                func_800CE610(D_800F33E0->pool);
            if (effect) {
                RoomM273PlayerTransform *transform = g_PlayerEntity->transform;
                D_8019AE5C = 1;
                effect->position = &transform->position;
            }
        }
        break;
    case 2:
        if (D_8019AE5C) {
            int palette = D_800E2850[D_800E11FA];
            D_800F3368 = 32;
            D_800F336A = 2;
            D_800F3376 = 32;
            D_800F3378 = 32;
            D_800F3376 = 32;
            D_800F3378 = 32;
            asm("" : : : "memory", "$2");
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3372 = 0;
            D_800F3374 = 0;
            D_800F3370 = palette;
        }
        break;
    }
    return 0;
}
