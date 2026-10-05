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
            RoomM273PlayerTransform *transform = player->tail.transform;
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

extern short D_8019AE60, D_8019AE64;
extern u16 D_800F3370, D_800F3372;

/* Keep this controller's scalar halfword views to preserve its retail register schedule. */
extern u16 RoomM273BurstParameter00 asm("D_800F3368");
extern u16 RoomM273BurstParameter02 asm("D_800F336A");

typedef GteShortVector Vector;

int func_80194470(int mode, Vector *input) {
    Vector *position = input;
    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        int firstFrame = D_800E27EC - 1;
        int frame;
        unsigned int offset;
        int kind = D_800F336C;
        int size, palette;
        unsigned short clut;
        asm("" : : "r"(firstFrame), "r"(position));
        offset = ((unsigned int)firstFrame << 9) & 0x3E00;
        frame = firstFrame;
        size = *(short *)((char *)D_800966EC + offset) * 2 + 4096;
        asm("" : : "r"(size), "r"(frame) : "memory");
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)((int *)D_800966EC)[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 6, 0);
    }
    return 0;
}

int func_801945A8(int mode) {
    switch(mode) {
    case 0:
        D_8019AE60=0; D_8019AE64=0;
        return func_800CE560(D_800F33E0->pool,8,4,func_80194470);
    case 1: {
        RoomM273EffectModeState *state;
        if(D_8019AE9A) return 2;
        state=D_800F32D0->state.mode;
        if(state->kind==9) {
            unsigned short value=state->value26;
            if(state->value22>=2 && (short)value<2) {
                D_8019AE60=4; D_8019AE64=0;
            }
        }
        if(D_8019AE60) {
            short old=D_8019AE64--;
            if(old<=0) {
                Vector *position=func_800CE610(D_800F33E0->pool);
                if(position) {
                    RoomM273PlayerTransform *transform=g_PlayerEntity->tail.transform;
                    position->x=transform->position.x;
                    position->y=transform->position.y;
                    position->z=transform->position.z;
                    D_8019AE64=2;
                    D_8019AE60--;
                }
            }
        }
        break;
    }
    case 2: {
        int palette=D_800E2850[D_800E11FA];
        RoomM273BurstParameter00=32; RoomM273BurstParameter02=2;
        D_800F3376=32; D_800F3378=32; D_800F3376=32; D_800F3378=32;
        asm("" : : : "memory", "$2");
        D_800F336C=3; D_800F336E=1; D_800F3372=0; D_800F3374=32;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}

int func_801947CC(int mode, RoomM273PaletteInput *input)
{
    RoomM273TrigEntry *entry;
    RoomM273PaletteEffect *effect;
    GteShortVector position;
    GteVector *source;
    int x;
    int y;
    int z;
    GteRotation rotation;
    int random;

    if (mode == 1) {
        int frame;
        frame = D_800E27EC;
        if (frame >= 33) return 1;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        input->size = (entry->low * 3 * 2048) / 4096 + 4096;
        if (frame >= 24) return 0;
        effect = func_800CE610(D_8019AE94);
        if (!effect) return 0;
        effect->source = input->source;
        effect->size = (input->size * 3 * 256) / 4096;
        effect->depth = Inv_ScrambleGrid() + 256;
        effect->color = D_8019AC30[D_800E27EC & 3];
        effect->x = Inv_ScrambleGrid() << 4;
        random = Inv_ScrambleGrid();
        effect->y = (128 - random) >> 2;
    } else if (mode == 2) {
        int frame;
        source = input->source;
        frame = D_800E27EC;
        y = source->y;
        x = source->x;
        frame = frame - 1;
        position.y = y;
        z = source->z;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        position.z = z;
        position.x = x;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 170 * frame;
        rotation.flags = 0;
        func_800D004C(&position, 256, 256, 12, &rotation,
                       input->size, input->size,
                       (RenderColor *)D_8019AB70,
                       (RenderColor *)&D_8019ABFC[D_8019AE98],
                       entry->high >> 5, 1);
    }
    return 0;
}


extern int func_80199F84();
extern int func_800CE5AC(void *, int, int, int, int (*)());
extern int func_800CE688(void *);
extern int func_800CE78C(void *);

int func_801949EC(int mode) {
    switch (mode) {
    case 0: {
        int size = func_800CE560(D_800F33E0->pool, 8, 2, func_801947CC);
        asm("" : : "r"(size));
        return size + func_800CE5AC(&D_8019AE94, size, 16, 9, func_80199F84);
    }
    case 1: {
        RoomM273EffectModeState *state;
        if (D_8019AE9A) return 2;
        state = D_800F32D0->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 >= 4 && (short)value < 4) {
                RoomM273PointerPoolEffect *effect = (RoomM273PointerPoolEffect *)func_800CE610(D_800F33E0->pool);
                if (effect) effect->position = g_PlayerEntity->tail.object + 20;
            }
        }
        func_800CE688(D_8019AE94);
        break;
    }
    case 2:
        func_800CE78C(D_8019AE94);
        break;
    }
    return 0;
}
