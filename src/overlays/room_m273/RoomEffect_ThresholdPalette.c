#include "room_m273.h"

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

typedef struct {
    u8 reserved_00[0x238];
    u8 *object;
} RoomM273PlayerObjectView;

typedef struct {
    void *position;
} RoomM273PoolEffect;

extern RoomM273EffectStateContext *D_800F32D0;
extern RoomM273PlayerObjectView *g_PlayerEntity;
extern u8 D_8019AE9A;
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
                RoomM273PoolEffect *effect = (RoomM273PoolEffect *)func_800CE610(D_800F33E0->pool);
                if (effect) effect->position = g_PlayerEntity->object + 20;
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
