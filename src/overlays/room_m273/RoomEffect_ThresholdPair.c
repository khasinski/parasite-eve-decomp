#include "room_m273.h"
#include "pe1/psyq_gpu.h"

extern RoomM273PlayerActorView *g_PlayerEntity;
extern RoomM273EffectStateContext *D_800F32D0;
extern u8 D_8019AE9A;
extern RoomM273PulseSeed D_8019AC20[2];
extern u16 D_800E11FA, D_800E2850[];
extern u16 D_800F336E, D_800F3370, D_800F3372;
extern u16 D_800F3376, D_800F3378;
extern int D_800E27EC;
extern int func_800CE560(void *, int, int, int (*)());
extern void *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);

int func_80194128(int mode, GteRotation *rotation) {
    GteShortVector position;
    int frame, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        RoomM273PlayerActorView *player;
        /* The loop index is reused for the sampled scale below. */
        size = 0;
        player = g_PlayerEntity;
        for (; size < 3; size++) {
            RoomM273PlayerTransform *transform = player->transform;
            ((s16 *)&position)[size] = ((s32 *)&transform->position.x)[size];
        }

        frame = D_800E27EC - 1;
        kind = D_800F336C;
        size = *(s16 *)((char *)D_800966EC + ((frame << 8) & 0x3F00));
        palette = D_800E1204[kind];

        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 11 : palette + 7);
        func_800CEE20(&position, rotation, 8192, size * 3, 5, clut, 1,
                       /* The upper half of the packed trig entry is signed. */
                       (s16)(*(s32 *)((char *)D_800966EC +
                           ((frame << 8) & 0x3F00)) >> 16) >> 5, 0);
    }
    return 0;
}

int func_80194284(int mode) {
    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 4, func_80194128);
    case 1: {
        RoomM273EffectModeState *state;
        if (D_8019AE9A) return 2;
        state = D_800F32D0->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 > 0 && (short)value <= 0) {
                short i = 0;
                int angle = Inv_ScrambleGrid() << 4;
                do {
                    RoomM273PulseSeed *seed = func_800CE610(D_800F33E0->pool);
                    if (!seed) break;
                    *seed = D_8019AC20[i];
                    i++;
                    seed->angle += angle;
                } while (i < 2);
            }
        }
        break;
    }
    case 2: {
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3376 = 16; D_800F3378 = 16; D_800F3376 = 16;
        palette = D_800E2850[D_800E11FA];
        D_800F336A = 1; D_800F3378 = 64;
        asm("" : : : "memory");
        D_800F336C = 3; D_800F336E = 1; D_800F3372 = 3; D_800F3374 = 0;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
