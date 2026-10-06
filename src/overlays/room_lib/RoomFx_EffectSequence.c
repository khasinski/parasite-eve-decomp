/*
 * The effect sequence: a configurable sequence of sprite effects (four
 * draw variants with their setups, paired sprites, sparkles, a glow and a
 * drifting effect) that the room's module drives.
 *
 * room_m049, m050, m090, m344 and m374 link the same twenty-five
 * functions in this order, at the same addresses, from the module's own
 * registration and start methods (which act whatever the field engine's
 * status, and close through the module's no-op) to the drifting effect
 * update,
 * with five seeds as their only read-only data (0x40 bytes after the room
 * header). This unit is that object, compiled into each of them. The
 * sprite records and packets it fills, and the module's lists
 * (pe1/room_module.h), are room data at the same addresses in all five
 * rooms.
 */
#include "common.h"
#include "pe1/room_effect_sequence.h"
#include "room_lib.h"
#include "RoomLib_Overlay024.h"
#include "pe1/field_script_context.h"
#include "pe1/room_fx.h"
#include "pe1/room_module.h"
#include "pe1/gte_types.h"

/* Seeds and scales copied into the draw passes' stack frames. */
static const RoomOverlay024MatrixSeed8 s_EffectBaseSeed = {{0x00, 0x04, 0, 0, 0, 0, 0, 0}};
static const RoomOverlay024Vec4 s_EffectWideScale = {0x96, 0x12C, 0x12C, 0};
static const RoomOverlay024Vec4 s_EffectNarrowScale = {0x46, 0x78, 0x46, 0};
static const RoomOverlay024MatrixSeed8 s_EffectProjectSeed = {{0x00, 0x00, 0x10, 0xFF, 0, 0, 0, 0}};
static const RoomOverlay024VectorSeed s_EffectVectorSeed = {{0x1388, 0xC18, 0x1388, 0}};

/* Class slot 3: registers the draw list. */
int RoomFx_EffectSequenceRegister(void *o) {
    FieldEng_Register(o, g_RoomDrawList);
    return 0;
}

/* Class slot 4: registers the update handlers and spawns the module's
 * objects, closing the module when either call fails. */
int RoomFx_EffectSequenceStart(void *o) {
    if ((func_800C251C(o, g_RoomUpdateList) |
         func_800C2758(o, g_RoomInitList, g_RoomSpawnLayout)) == -1) {
        RoomFx_EffectSequenceNop5(o);
    }

    return 0;
}

/* Class slot 5: the module closes without touching its target. */
s32 RoomFx_EffectSequenceNop5(void *o) {
    return 0;
}

s32 RoomFx_EffectSequenceNop6(void) {
    return 0;
}

void RoomFx_InitEffectSequenceSprites(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
    RoomLink *link;
    RoomLibFxMatrixWords *matrix;
    register int c80a asm("$6");
    register int c80b asm("$3");
    register int c40 asm("$5");
    int c20;
    register int c2 asm("$2");

    func_800C2B40(state);
    D_80192C34 = func_8006E498(D_800B0E64, 0x498704);
    link = ent->link;
    state->link = link;
    matrix = (RoomLibFxMatrixWords *)link->p238;
    state->matrix = *matrix;
    state->asset = func_8006DC18(0xE);

    D_80192C0C = 0;
    D_80192C0D = 0;
    D_80192C10 = 1;
    if (*func_800C2B28(0) == 2 || *func_800C2B28(0) == 5) {
        D_80192C10 = -100;
    }
    c80a = 0x80;
    c80b = 0x80;
    PE1_COMPILER_USE(c80b);
    c40 = 0x40;
    c20 = 0x20;
    PE1_COMPILER_USE(c20);
    D_80192C1D = 3;
    D_80192C20 = -100;
    D_80192C2C = 8;
    PE1_COMPILER_MEMORY_BARRIER();
    c2 = 2;
    D_80192C09 = c40;
    D_80192C19 = c40;
    c40 = 2;
    D_80192C2D = c2;
    D_80192C28 = 0x90;
    D_80192C29 = 0x90;
    D_80192C2A = 0x90;
    D_80192C12 = c80a;
    D_80192C0E = 0;
    D_80192C08 = c80b;
    D_80192C0A = c20;
    D_80192C1C = c80b;
    D_80192C22 = c80a;
    D_80192C1E = 0;
    D_80192C18 = c80b;
    D_80192C1A = c20;
    D_80192C30 = c40;
    D_80192C32 = c80a;
    D_80192C2E = 0;
    D_80192C3C = 0;
    D_80192C3D = 0;
    D_80192C40 = c40;
    D_80192C3E = 0;
    D_80192C42 = c80a;
    D_80192C38 = c80b;
    D_80192C39 = c20;
    D_80192C3A = 0x10;
    ent->pad1[1] = *func_800C2B28(0);
}

void RoomFx_EffectSequenceNopA(void) {
}

#define ROOMLIB_EFFECT_CALL(command) \
    func_800C2B90(owner, (command), &D_80192BBC, &D_80192B5C)
#define ROOMLIB_EFFECT_SET(index, value, command) \
    *func_800C2B10(index) = (value); \
    ROOMLIB_EFFECT_CALL(command)

void RoomFx_ConfigureEffectSequence(int owner, void *signal)
{
    short counter;
    int mode3;
    int mode4;

    if (*func_800C2B28(0) == 0) {
        if (RW16(signal, 2) == 0) {
            *func_800C2B10(1) = 0;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        counter = RW16(signal, 2);
        if (counter == 10) {
            *func_800C2B10(1) = counter;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 20) {
            *func_800C2B10(1) = 8;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 30) {
            *func_800C2B10(1) = 15;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 40) {
            *func_800C2B10(1) = 4;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 50) {
            *func_800C2B10(1) = 18;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 60) {
            *func_800C2B10(1) = 2;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 70) {
            *func_800C2B10(1) = 9;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 80) {
            *func_800C2B10(1) = 3;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 90) {
            *func_800C2B10(1) = 19;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 100) {
            *func_800C2B10(1) = 20;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 110) {
            *func_800C2B10(1) = 22;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 120) {
            *func_800C2B10(1) = 12;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 130) {
            *func_800C2B10(1) = 17;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
    }

    if (*func_800C2B28(0) == 1) {
        if (RW16(signal, 2) == 0) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 80, 1);
        }
        if (RW16(signal, 2) == 3) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 80, 1);
        }
        if (RW16(signal, 2) == 6) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 40, 1);
        }
        if (RW16(signal, 2) == 9) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 40, 1);
        }
        if (RW16(signal, 2) == 12) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 20, 1);
        }
        if (RW16(signal, 2) == 15) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 20, 1);
        }
    }

    if (*func_800C2B28(0) == 2 && RW16(signal, 2) == 0) {
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(5);
        ROOMLIB_EFFECT_SET(1, 800, 4);
        ROOMLIB_EFFECT_SET(1, 700, 4);
        ROOMLIB_EFFECT_SET(1, 600, 4);
        ROOMLIB_EFFECT_SET(1, 500, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 200, 4);
        ROOMLIB_EFFECT_SET(1, 100, 4);
    }

    if (*func_800C2B28(0) == 5 && RW16(signal, 2) == 0) {
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(5);
        ROOMLIB_EFFECT_SET(1, 1000, 4);
        ROOMLIB_EFFECT_SET(1, 800, 4);
        ROOMLIB_EFFECT_SET(1, 600, 4);
        ROOMLIB_EFFECT_SET(1, 500, 4);
        ROOMLIB_EFFECT_SET(1, 400, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 200, 4);
        ROOMLIB_EFFECT_SET(1, 100, 4);
    }

    mode3 = *func_800C2B28(0);
    if (mode3 == 3 && RW16(signal, 2) == 0) {
        *func_800C2B10(2) = -1;
        ROOMLIB_EFFECT_SET(1, 1, 3);
        ROOMLIB_EFFECT_SET(1, mode3, 3);
        ROOMLIB_EFFECT_SET(1, 5, 3);
        ROOMLIB_EFFECT_SET(1, 7, 3);
        ROOMLIB_EFFECT_SET(1, 9, 3);
        ROOMLIB_EFFECT_SET(1, 11, 3);
        ROOMLIB_EFFECT_SET(1, 13, 3);
        ROOMLIB_EFFECT_SET(1, 15, 3);
        ROOMLIB_EFFECT_SET(1, 17, 3);
        ROOMLIB_EFFECT_SET(1, 19, 3);
        ROOMLIB_EFFECT_SET(1, 21, 3);
        ROOMLIB_EFFECT_SET(1, 0, 3);
        ROOMLIB_EFFECT_SET(1, 2, 3);
        ROOMLIB_EFFECT_SET(1, 4, 3);
        ROOMLIB_EFFECT_SET(1, 6, 3);
        ROOMLIB_EFFECT_SET(1, 8, 3);
    }

    mode4 = *func_800C2B28(0);
    if (mode4 == 4 && RW16(signal, 2) == 0) {
        *func_800C2B10(2) = -1;
        ROOMLIB_EFFECT_SET(1, 2, 2);
        ROOMLIB_EFFECT_SET(1, mode4, 2);
        ROOMLIB_EFFECT_SET(1, 6, 2);
        ROOMLIB_EFFECT_SET(1, 8, 2);
        ROOMLIB_EFFECT_SET(1, 10, 2);
        ROOMLIB_EFFECT_SET(1, 12, 2);
        ROOMLIB_EFFECT_SET(1, 14, 2);
        ROOMLIB_EFFECT_SET(1, 16, 2);
        ROOMLIB_EFFECT_SET(1, 17, 2);
        ROOMLIB_EFFECT_SET(1, 18, 2);
        ROOMLIB_EFFECT_SET(1, 20, 2);
        ROOMLIB_EFFECT_SET(1, 22, 2);
        ROOMLIB_EFFECT_SET(1, mode4, 2);
        ROOMLIB_EFFECT_SET(1, 6, 2);
        ROOMLIB_EFFECT_SET(1, 8, 2);
        ROOMLIB_EFFECT_SET(1, 10, 2);
        ROOMLIB_EFFECT_SET(1, 12, 2);
        ROOMLIB_EFFECT_SET(1, 14, 2);
        ROOMLIB_EFFECT_SET(1, 16, 2);
        ROOMLIB_EFFECT_SET(1, 17, 2);
        ROOMLIB_EFFECT_SET(1, 18, 2);
        ROOMLIB_EFFECT_SET(1, 20, 2);
        ROOMLIB_EFFECT_SET(1, 21, 2);
    }

    if (func_800C2B68() == 1) {
        RW8(signal, 1) = 2;
    }
}

s32 *func_800C2B10(s32 index);

void RoomFx_InitVariant38(void *arg0, void *arg1, RoomOverlay024Variant38SetupState *state) {
    state->transform_index = *func_800C2B10(1);
    state->resource_selector = *func_800C2B10(2);
    state->active_flag = 1;

    if (state->resource_selector == -1) {
        state->width = 0;
        state->height = 0;
    } else {
        state->width = 0x80;
        state->height = 0x38;
    }

    state->field2C = 0;
    state->field30 = 0;
    state->field22 = 0;
}

void func_80071A44(void *arg0, s32 arg1, s32 arg2);

void RoomFx_DrawVariant38(void *arg0, RoomOverlay024VariantControl *control, RoomOverlay024Variant38State *state) {
    RoomOverlay024Root **owner_ptr;
    RoomOverlay024Root *owner;
    RoomOverlay024Transform *transform;
    RoomOverlay024Matrix matrix;
    RoomOverlay024MatrixSeed8 base_seed;
    RoomOverlay024Vec4 scratch_scale;
    RoomOverlay024Vec4 draw_scale;
    RoomOverlay024Vec4 second_scale;
    RoomOverlay024Vec4 third_scale;
    volatile s16 *fade_slot;
    u8 *phase_slot;
    char *draw_slot;
    s16 *sparkle_alpha_slot;
    s16 *final_alpha_slot;
    s32 draw_zero;
    volatile RoomOverlay024Transform *ordered_transform;

    owner_ptr = func_800C2B50();
    owner = *owner_ptr;
    transform = &((RoomOverlay024Transform *)owner->view)[state->transform_index];
    base_seed = s_EffectBaseSeed;

    func_800C2EAC(*(u8 *)((char *)owner_ptr + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    if (state->active_flag == 0) {
        return;
    }

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = state->drift_value;
    draw_scale.y = state->drift_value;
    draw_scale.z = state->drift_value;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    fade_slot = &D_80192C12;
    *fade_slot = state->fade_alpha;

    matrix.t[0] = transform->x;
    matrix.t[1] = transform->y - 0x64;
    matrix.t[2] = transform->z;

    if (state->resource_selector == -1) {
        func_800C3134(&D_80192BF0, state->field30, (char *)fade_slot - 0xA);
    } else {
        func_800C3134(&D_80192BFC, state->field30, (char *)fade_slot - 0xA);
    }
    phase_slot = &D_80192C0C;
    draw_slot = (char *)phase_slot - 4;
    *phase_slot = state->phase * 2;
    func_800C42A4(draw_slot, &matrix, 1);
    RotMatrix(&base_seed, &matrix);

    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = state->drift_value;
    second_scale.y = state->drift_value;
    second_scale.z = state->drift_value;
    draw_scale = second_scale;
    ScaleMatrix(&matrix, &draw_scale);

    ordered_transform = transform;
    D_80192C12 = state->fade_alpha >> 2;
    matrix.t[0] = ordered_transform->x;
    matrix.t[1] = D_800942EC;
    draw_zero = 0;
    matrix.t[2] = ordered_transform->z;
    *phase_slot = 8;
    func_800C42A4(draw_slot, &matrix, draw_zero);

    if (control->frame >= 0x5B && state->sparkle_timer > 0) {
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;

        second_scale = s_EffectWideScale;
        ScaleMatrix(&matrix, &second_scale);

        matrix.t[0] = state->sparkle_x;
        matrix.t[1] = state->sparkle_y;
        matrix.t[2] = state->sparkle_z;
        sparkle_alpha_slot = &D_80192C42;
        *sparkle_alpha_slot = state->sparkle_alpha;
        func_800C42A4((char *)sparkle_alpha_slot - 0xA, &matrix, 0);
    }

    if (control->frame < 0x1F) {
        return;
    }

    func_800C3238(3);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&third_scale, 0, 0x10);
    third_scale.x = state->final_scale >> 1;
    third_scale.y = state->final_scale;
    third_scale.z = state->final_scale;
    second_scale = third_scale;
    ScaleMatrix(&matrix, &second_scale);

    matrix.t[0] = state->final_x;
    matrix.t[1] = state->final_y;
    matrix.t[2] = state->final_z;
    final_alpha_slot = &D_80192C32;
    *final_alpha_slot = state->final_alpha;
    func_800C42A4((char *)final_alpha_slot - 0xA, &matrix, 1);
}

int rand(void);

void RoomFx_UpdatePairedSprite(void *unused,
                                       RoomFxControl *control,
                                       RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];

    state->phase++;
    state->phase %= 4;

    if (state->counter < 200) {
        state->counter++;
    }

    if (state->resourceSelector > 0) {
        state->resourceSelector--;
        if (state->resourceSelector == 0) {
            state->active = 0;
        }
    }

    if ((control->frame % 30) == 0) {
        state->x = transform->x;
        state->y = transform->y;
        state->z = transform->z;
        state->velocityX = 0;
        state->velocityY = -5;
        state->velocityZ = 0;
        state->alpha = 0x80;
        state->alphaStep = 0x96;
    }

    state->x += state->velocityX;
    state->y += state->velocityY;
    state->z += state->velocityZ;
    state->alphaStep += 60;
    state->alpha -= 4;
    if (state->alpha < 0) {
        state->alpha = 0;
    }

    if ((control->frame % 300) == 0) {
        state->sparkleX = transform->x;
        state->sparkleY = transform->y;
        state->sparkleZ = transform->z;
        state->sparkleAlpha = 0xFF;
        state->sparkleLife = 20;
    }

    if (state->sparkleLife > 0) {
        int random;
        int life;
        int alpha;
        int y;

        random = rand();
        life = *(unsigned short *)&state->sparkleLife;
        alpha = state->sparkleAlpha;
        y = state->sparkleY;
        life--;
        alpha -= 8;
        y -= 5;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 20;
        state->sparkleY = y;
    }

    if (state->height < 1000) {
        state->height += rand() % 50;
    }

    if (state->height >= 501) {
        state->height -= rand() % 30;
    }

    if (state->resourceSelector == -1 && state->width != 0x80) {
        state->width += 8;
    }

    if ((unsigned short)(state->resourceSelector - 1) < 31) {
        state->width -= 4;
        if (state->width < 0) {
            state->width = 0;
        }
    }
}

s32 func_80071A54(void);

void RoomFx_InitVariant38Pulse(void *arg0, void *arg1, RoomOverlay024Variant38SetupState *state) {
    s32 random;

    state->transform_index = *func_800C2B10(1);
    state->resource_selector = *func_800C2B10(2);
    state->active_flag = 1;

    if (state->resource_selector == -1) {
        state->width = 0;
        state->height = 0;
    } else {
        state->width = 0x80;
        state->height = 0x38;
    }

    state->field2C = 0;
    state->field30 = 0;
    state->field22 = 0;
    state->sparkle_timer = 0;
    random = func_80071A54();
    state->random_mod = random % 40;
}

void RoomFx_DrawVariant38Pulse(void *arg0, RoomOverlay024VariantControl *control, RoomOverlay024Variant38State *state) {
    RoomOverlay024Root **owner_ptr;
    RoomOverlay024Root *owner;
    RoomOverlay024Transform *transform;
    RoomOverlay024Matrix matrix;
    RoomOverlay024MatrixSeed8 base_seed;
    RoomOverlay024Vec4 scratch_scale;
    RoomOverlay024Vec4 draw_scale;
    RoomOverlay024Vec4 second_scale;
    RoomOverlay024Vec4 third_scale;
    s16 *fade_slot;
    u8 *phase_slot;
    char *draw_slot;
    s16 *sparkle_alpha_slot;
    s16 *final_alpha_slot;
    s32 draw_zero;
    volatile RoomOverlay024Transform *ordered_transform;

    owner_ptr = func_800C2B50();
    owner = *owner_ptr;
    transform = &((RoomOverlay024Transform *)owner->view)[state->transform_index];
    base_seed = s_EffectBaseSeed;

    func_800C2EAC(*(u8 *)((char *)owner_ptr + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    if (state->active_flag == 0) {
        return;
    }

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = state->drift_value;
    draw_scale.y = state->drift_value >> 1;
    draw_scale.z = state->drift_value;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    fade_slot = (s16 *)&D_80192C12;
    *fade_slot = state->fade_alpha >> 1;

    matrix.t[0] = transform->x + (func_80071A54() % 20) - 10;
    matrix.t[1] = transform->y;
    matrix.t[2] = transform->z + (func_80071A54() % 20) - 10;

    if (state->resource_selector == -1) {
        func_800C3134(&D_80192BF0, state->field30, (char *)fade_slot - 0xA);
    } else {
        func_800C3134(&D_80192BFC, state->field30, (char *)fade_slot - 0xA);
    }
    phase_slot = &D_80192C0C;
    draw_slot = (char *)phase_slot - 4;
    *phase_slot = state->phase * 2;
    func_800C42A4(draw_slot, &matrix, 1);
    RotMatrix(&base_seed, &matrix);

    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = state->drift_value * 2;
    second_scale.y = state->drift_value;
    second_scale.z = state->drift_value * 2;
    draw_scale = second_scale;
    ScaleMatrix(&matrix, &draw_scale);

    ordered_transform = transform;
    D_80192C12 = state->fade_alpha >> 2;
    matrix.t[0] = ordered_transform->x;
    matrix.t[1] = D_800942EC;
    draw_zero = 0;
    matrix.t[2] = ordered_transform->z;
    *phase_slot = 8;
    func_800C42A4(draw_slot, &matrix, draw_zero);

    if (control->frame >= 0x5B && state->sparkle_timer > 0) {
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;

        second_scale = s_EffectNarrowScale;
        ScaleMatrix(&matrix, &second_scale);

        matrix.t[0] = state->sparkle_x;
        matrix.t[1] = state->sparkle_y;
        matrix.t[2] = state->sparkle_z;
        sparkle_alpha_slot = &D_80192C42;
        *sparkle_alpha_slot = state->sparkle_alpha;
        func_800C42A4((char *)sparkle_alpha_slot - 0xA, &matrix, 0);
    }

    func_800C3238(3);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&third_scale, 0, 0x10);
    third_scale.x = state->final_scale;
    third_scale.y = state->final_scale;
    third_scale.z = state->final_scale;
    second_scale = third_scale;
    ScaleMatrix(&matrix, &second_scale);

    matrix.t[0] = state->final_x;
    matrix.t[1] = state->final_y;
    matrix.t[2] = state->final_z;
    final_alpha_slot = &D_80192C32;
    *final_alpha_slot = state->final_alpha;
    func_800C42A4((char *)final_alpha_slot - 0xA, &matrix, 1);
}

void RoomFx_UpdatePairedSpriteBurst(
    void *unused, void *unusedControl, RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];

    state->phase++;
    state->phase %= 4;

    if (state->counter < 200) {
        state->counter++;
    }

    if (state->resourceSelector > 0) {
        state->resourceSelector--;
        if (state->resourceSelector == 0) {
            state->active = 0;
        }
    }

    state->sparkleTimer++;
    if (state->alpha == 0) {
        int random;

        state->sparkleTimer = 0;
        state->x = transform->x;
        state->y = transform->y;
        state->z = transform->z;
        state->velocityX = 0;
        state->velocityY = -8;
        state->velocityZ = 0;
        random = rand();
        state->alphaStep = 150;
        state->alpha = (random % 88) + 40;
    }

    state->x += state->velocityX;
    state->y += state->velocityY;
    state->z += state->velocityZ;

    {
        int random;
        int alpha;
        int alphaStep;

        random = rand();
        alpha = *(unsigned short *)&state->alpha;
        alphaStep = state->alphaStep;
        alpha -= 4;
        alphaStep += 50;
        state->alpha = alpha;
        alphaStep += random % 30;
        state->alphaStep = alphaStep;
        if ((short)alpha < 0) {
            state->alpha = 0;
        }
    }

    if ((rand() % 200) == 0) {
        state->sparkleX = transform->x;
        state->sparkleY = transform->y;
        state->sparkleZ = transform->z;
        state->sparkleAlpha = 0xFF;
        state->sparkleLife = 20;
    }

    if (state->sparkleLife > 0) {
        int random;
        int life;
        int alpha;
        int y;

        random = rand();
        life = *(unsigned short *)&state->sparkleLife;
        alpha = state->sparkleAlpha;
        y = state->sparkleY;
        life--;
        alpha -= 8;
        y -= 2;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 5;
        state->sparkleY = y;
    }

    if (state->height < 500) {
        state->height += rand() % 20;
    }

    if (state->height >= 401) {
        state->height -= rand() % 15;
    }

    if (state->resourceSelector == -1 && state->width != 0x80) {
        state->width += 8;
    }

    if ((unsigned short)(state->resourceSelector - 1) < 31) {
        state->width -= 4;
        if (state->width < 0) {
            state->width = 0;
        }
    }
}

void RoomFx_InitVariant290(void *arg0, void *arg1, RoomOverlay024Variant290SetupState *state) {
    s32 random;
    s32 height;

    state->transform_index = *func_800C2B10(1);
    state->resource_selector = *func_800C2B10(2);
    state->active_flag = 1;

    if (state->resource_selector == -1) {
        height = 0x290;
        state->width = 0;
    } else {
        state->width = 0x80;
        height = 0x290;
    }
    state->height = height;

    state->field2C = 0;
    state->field30 = 0;
    state->field22 = 0;
    state->sparkle_timer = 0;
    random = func_80071A54();
    state->random_mod = random % 40;
}

void RoomFx_DrawVariant290(void *arg0, RoomOverlay024VariantControl *control, RoomOverlay024Variant290State *state) {
    RoomOverlay024Root **owner_ptr;
    RoomOverlay024Root *owner;
    RoomOverlay024Transform *transform;
    RoomOverlay024Matrix matrix;
    RoomOverlay024MatrixSeed8 base_seed;
    RoomOverlay024Vec4 scratch_scale;
    RoomOverlay024Vec4 draw_scale;
    RoomOverlay024Vec4 second_scale;
    s16 *fade_slot;
    s16 *sparkle_alpha_slot;

    owner_ptr = func_800C2B50();
    owner = *owner_ptr;
    transform = &((RoomOverlay024Transform *)owner->view)[state->transform_index];
    base_seed = s_EffectBaseSeed;

    func_800C2EAC(*(u8 *)((char *)owner_ptr + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    if (state->active_flag == 0) {
        return;
    }

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = state->drift_value;
    draw_scale.y = state->drift_value;
    draw_scale.z = state->drift_value;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    fade_slot = &D_80192C12;
    *fade_slot = state->fade_alpha;

    matrix.t[0] = transform->x + (func_80071A54() % 20) - 10;
    matrix.t[1] = transform->y;
    matrix.t[2] = transform->z + (func_80071A54() % 20) - 10;

    func_800C3134(&D_80192BFC, state->field30, (char *)fade_slot - 0xA);
    D_80192C0C = state->phase * 2;
    func_800C42A4((char *)fade_slot - 0xA, &matrix, 1);
    RotMatrix(&base_seed, &matrix);

    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = state->drift_value * 2;
    second_scale.y = state->drift_value;
    second_scale.z = state->drift_value * 4;
    draw_scale = second_scale;
    ScaleMatrix(&matrix, &draw_scale);

    *fade_slot = state->fade_alpha >> 2;
    matrix.t[0] = transform->x;
    matrix.t[1] = D_800942EC;
    matrix.t[2] = transform->z;
    D_80192C0C = 8;
    func_800C42A4((char *)fade_slot - 0xA, &matrix, 0);

    if (control->frame < 0x5B) {
        return;
    }
    if (state->sparkle_timer <= 0) {
        return;
    }

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    second_scale = s_EffectNarrowScale;
    ScaleMatrix(&matrix, &second_scale);

    matrix.t[0] = state->sparkle_x;
    matrix.t[1] = state->sparkle_y;
    matrix.t[2] = state->sparkle_z;
    sparkle_alpha_slot = &D_80192C42;
    *sparkle_alpha_slot = state->sparkle_alpha;
    func_800C42A4((char *)sparkle_alpha_slot - 0xA, &matrix, 0);
}

void RoomFx_UpdateSparkleHeight(
    void *unused, RoomFxControl *control, RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];

    state->phase++;
    state->phase %= 4;

    if (state->counter < 200) {
        state->counter++;
    }

    if (state->resourceSelector > 0) {
        state->resourceSelector--;
        if (state->resourceSelector == 0) {
            state->active = 0;
        }
    }

    if ((rand() % 200) == 0) {
        state->sparkleX = transform->x;
        state->sparkleY = transform->y;
        state->sparkleZ = transform->z;
        state->sparkleAlpha = 0xFF;
        state->sparkleLife = 20;
    }

    if (state->sparkleLife > 0) {
        int random;
        int life;
        int alpha;
        int y;

        random = rand();
        life = *(unsigned short *)&state->sparkleLife;
        alpha = state->sparkleAlpha;
        y = state->sparkleY;
        life--;
        alpha -= 8;
        y -= 10;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 20;
        state->sparkleY = y;
    }

    if (control->frame >= 31) {
        state->height -= rand() % 20;
        state->height += rand() % 2;
        if (state->height < 500) {
            state->height = 500;
        }
    } else {
        if (state->height < 1300) {
            state->height += rand() % 60;
        }
        if (state->height >= 1101) {
            state->height -= rand() % 40;
        }
    }

    if (state->resourceSelector == -1 && state->width != 0x80) {
        state->width += 8;
    }

    if ((unsigned short)(state->resourceSelector - 1) < 31) {
        state->width -= 4;
        if (state->width < 0) {
            state->width = 0;
        }
    }
}

s32 *func_800C2B10(s32 slot);

void RoomFx_InitVariantD(void *arg0, void *arg1, RoomOverlay024VariantDSetupRecord *obj) {
    s32 random;

    obj->half2E = 0xD;
    obj->resource1 = *func_800C2B10(1);
    obj->resource2 = *func_800C2B10(2);
    obj->half26 = 1;
    obj->half2A = 0x80;
    obj->resource1_again = *func_800C2B10(1);
    obj->half2C = 0;
    obj->word30 = 0;
    obj->half22 = 0;
    obj->half14 = 0;
    random = func_80071A54();
    obj->random_mod = random % 40;
}

void RoomFx_DrawVariantD(void *arg0, RoomOverlay024VariantControl *control, RoomOverlay024VariantDState *state) {
    RoomOverlay024Root **owner_ptr;
    RoomOverlay024Root *owner;
    RoomOverlay024Transform *transform;
    RoomOverlay024Matrix matrix;
    RoomOverlay024MatrixSeed8 base_seed;
    RoomOverlay024MatrixSeed8 project_seed;
    s16 projected[4];
    RoomOverlay024Vec4 scratch_scale;
    RoomOverlay024Vec4 draw_scale;
    RoomOverlay024Vec4 second_scale;
    s16 *fade_slot;
    s16 *sparkle_alpha_slot;
    s32 scale;

    owner_ptr = func_800C2B50();
    owner = *owner_ptr;
    transform = &((RoomOverlay024Transform *)owner->view)[state->transform_index];
    base_seed = s_EffectBaseSeed;
    project_seed = s_EffectProjectSeed;

    func_800C2EAC(*(u8 *)((char *)owner_ptr + 0x24));
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    ApplyMatrixSV(transform, &project_seed, projected);

    scale = ((s16)state->drift_value) >> 2;
    if (state->active_flag == 0) {
        return;
    }

    func_800C2FF0(0x20, 0x20);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = scale * 3;
    draw_scale.y = state->drift_value;
    draw_scale.z = scale * 3;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    fade_slot = &D_80192C22;
    *fade_slot = state->fade_alpha;

    matrix.t[0] = transform->x + projected[0] + (func_80071A54() % 20) - 10;
    matrix.t[1] = transform->y + projected[1] - (((s16)state->vertical_offset) >> 4);
    matrix.t[2] = transform->z + projected[2] + (func_80071A54() % 20) - 10;

    func_800C3134(&D_80192BFC, state->field30, (char *)fade_slot - 0xA);
    D_80192C1C = state->phase * 2;
    func_800C42A4((char *)fade_slot - 0xA, &matrix, 1);

    func_800C2FF0(0x20, 0x20);
    RotMatrix(&base_seed, &matrix);

    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = state->drift_value;
    second_scale.y = state->drift_value;
    second_scale.z = state->drift_value;
    draw_scale = second_scale;
    ScaleMatrix(&matrix, &draw_scale);

    *fade_slot = state->fade_alpha >> 2;
    matrix.t[0] = transform->x + projected[0];
    matrix.t[1] = -0x457;
    matrix.t[2] = transform->z + projected[2];
    D_80192C1C = 8;
    func_800C42A4((char *)fade_slot - 0xA, &matrix, 0);

    if (control->frame < 0x5B) {
        return;
    }
    if (state->sparkle_timer <= 0) {
        return;
    }

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    second_scale = s_EffectNarrowScale;
    ScaleMatrix(&matrix, &second_scale);

    matrix.t[0] = state->sparkle_x;
    matrix.t[1] = state->sparkle_y;
    matrix.t[2] = state->sparkle_z;
    sparkle_alpha_slot = &D_80192C42;
    *sparkle_alpha_slot = state->sparkle_alpha;
    func_800C42A4((char *)sparkle_alpha_slot - 0xA, &matrix, 0);
}

void RoomFx_UpdateTransformedSparkle(
    void *unused, void *unusedControl, RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;
    RoomFxSeed8 seed;
    RoomFxSeed8 transformed;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];
    seed = s_EffectProjectSeed;
    ApplyMatrixSV(transform, &seed, (unsigned short *)&transformed);

    state->phase++;
    state->phase %= 4;

    if (state->counter < 200) {
        state->counter++;
    }

    if (state->resourceSelector > 0) {
        state->resourceSelector--;
        if (state->resourceSelector == 0) {
            state->active = 0;
        }
    }

    if ((rand() % 400) == 0) {
        state->sparkleX = *(unsigned short *)&transformed.bytes[0] +
                          transform->x + (rand() % 60) - 30;
        state->sparkleY = *(unsigned short *)&transformed.bytes[2] +
                          transform->y;
        {
            int random;
            int z;
            int transformZ;
            int sign;

            random = rand();
            transformZ = transform->z;
            z = *(unsigned short *)&transformed.bytes[4];
            asm("" : "=r"(transformZ), "=r"(z)
                   : "0"(transformZ), "1"(z));
            state->sparkleAlpha = 0xFF;
            state->sparkleLife = 20;
            sign = random >> 31;
            asm("" : "=r"(z)
                   : "0"(z), "m"(state->sparkleAlpha),
                     "m"(state->sparkleLife), "r"(sign));
            z += transformZ;
            random %= 60;
            state->sparkleZ = z + random - 30;
        }
    }

    if (state->sparkleLife > 0) {
        int random;
        int life;
        int alpha;
        int y;

        random = rand();
        life = *(unsigned short *)&state->sparkleLife;
        alpha = state->sparkleAlpha;
        y = state->sparkleY;
        life--;
        alpha -= 8;
        y -= 10;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 20;
        state->sparkleY = y;
    }

    state->height += (rand() % 30) - 15;
    if (state->height < state->minimumHeight) {
        state->height = state->minimumHeight;
    }

    if (state->resourceSelector == -1 && state->width != 0x80) {
        state->width += 8;
    }

    if ((unsigned short)(state->resourceSelector - 1) < 31) {
        state->width -= 4;
        if (state->width < 0) {
            state->width = 0;
        }
    }
}

void RoomFx_EffectSequenceNopB(void) {
}

void func_800C6D5C(void *arg0, s32 arg1, s32 arg2);
void func_800C6EE8(s32 arg0);
s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80077AA4(s32 arg0, s32 arg1);
void func_800C6EC0(s32 arg0, s32 arg1);
void func_800C6ED8(s32 arg0);
void func_800C6EF8(void *arg0);
void func_800C6FA0(void *arg0, s32 arg1);
void func_800C71E4(void *arg0, RoomOverlay024ViewMatrix *matrix);
void func_800C6F4C(void *arg0);

void RoomFx_DrawEffectGlow(void) {
    RoomOverlay024Root **root;
    RoomOverlay024View *view;
    RoomOverlay024ViewMatrix matrix;
    RoomOverlay024MatrixSeed8 seed;
    short out[4];
    RoomOverlay024VectorSeed vector_seed;
    s32 handle_a;
    s32 handle_b;

    root = func_800C2B50();
    view = (RoomOverlay024View *)(*root)->view;

    seed = s_EffectProjectSeed;
    matrix = view->tail.matrix;
    vector_seed = s_EffectVectorSeed;

    ScaleMatrix(&matrix, &vector_seed);
    ApplyMatrixSV(&view->tail.matrix, &seed, out);

    matrix.tx += out[0];
    matrix.ty += out[1];
    matrix.tz += out[2];

    func_800C2EAC(*(u8 *)((char *)root + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C6D5C(D_80192C34, 0, 0);
    func_800C6EE8(-10);
    handle_a = func_80077A64(0, 1, 0x340, 0x100);
    handle_a = (u16)handle_a;
    handle_b = func_80077AA4(0x10, 0x1D7);
    func_800C6EC0(handle_a, (u16)handle_b);
    func_800C6ED8(0);
    func_800C6EF8(D_80192C34);
    func_800C6FA0(D_80192C34, 0x80);
    func_800C71E4(D_80192C34, &matrix);
    func_800C6F4C(D_80192C34);
}

void RoomFx_EffectSequenceNopC(void) {
}

void RoomFx_SpawnDriftingEffect(void *arg0, void *arg1, RoomOverlay024Effect *effect) {
    RoomOverlay024View *view;
    RoomOverlay024MatrixSeed8 seed;
    u16 out[4];
    s32 random;

    view = (RoomOverlay024View *)(*(RoomOverlay024Root **)func_800C2B50())->view;
    seed = s_EffectProjectSeed;
    ApplyMatrixSV(&view->tail.position.transform, &seed, out);

    if (effect->timer == 0) {
        effect->half14 = 0;
        effect->x = out[0] + view->tail.position.base_x;
        effect->y = out[1] + view->tail.position.base_y;
        effect->z = out[2] + view->tail.position.base_z;
        effect->half8 = 0;
        effect->velocity_y = -8;
        effect->halfC = 0;
        random = func_80071A54();
        effect->duration = 0x96;
        effect->timer = random % 80 + 0x50;
    }
}

typedef struct RoomLibEffectMatrix {
    short m[3][3];
    short pad12;
    int t[3];
} RoomLibEffectMatrix;

typedef struct RoomLibEffectView {
    char pad00[0x1A0];
    RoomLibEffectMatrix matrix;
} RoomLibEffectView;

typedef struct RoomLibEffectRoot {
    char pad00[0x238];
    RoomLibEffectView *view;
} RoomLibEffectRoot;

typedef struct RoomLibEffectRootTable {
    RoomLibEffectRoot *root;
    char pad04[0x20];
    unsigned char mode;
} RoomLibEffectRootTable;

typedef struct RoomLibScaledEffect {
    short x;
    short y;
    short z;
    char pad06[0xA];
    unsigned short depth;
    unsigned short scaleTimer;
} RoomLibScaledEffect;

typedef RoomFxSeed8 RoomLibMatrixSeed;

typedef RoomOverlay024VectorSeed RoomLibVectorSeed;

void RoomFx_DrawScaledEffect(void *unused0, void *unused1, RoomLibScaledEffect *effect) {
    RoomLibEffectRootTable *roots;
    RoomLibEffectView *view;
    RoomLibEffectMatrix matrix;
    RoomLibMatrixSeed seed;
    short offset[4];
    RoomLibVectorSeed vectorSeed;
    RoomLibVectorSeed scaleSeed;
    unsigned short *depth;
    int scale;

    roots = (RoomLibEffectRootTable *)func_800C2B50();
    view = roots->root->view;
    seed = s_EffectProjectSeed;
    matrix = view->matrix;
    vectorSeed = s_EffectVectorSeed;
    ScaleMatrix((RoomSpriteMatrix *)&matrix, (RoomFxVec4 *)&vectorSeed);
    ApplyMatrixSV(&view->matrix, &seed, offset);
    matrix.t[0] += offset[0];
    matrix.t[1] += offset[1];
    matrix.t[2] += offset[2];

    func_800C2EAC(roots->mode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C3238(3);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    scale = (short)effect->scaleTimer >> 2;
    func_80071A44(&scaleSeed, 0, 0x10);
    scaleSeed.words[0] = scale * 3;
    scaleSeed.words[1] = scale * 3;
    scaleSeed.words[2] = scale * 3;
    ScaleMatrix((RoomSpriteMatrix *)&matrix, (RoomFxVec4 *)&scaleSeed);

    depth = (unsigned short *)&D_80192C32;
    matrix.t[0] = effect->x;
    matrix.t[1] = effect->y;
    matrix.t[2] = effect->z;
    D_80192C30 = -0x65;
    *depth = effect->depth;
    func_800C42A4((char *)depth - 0xA, (RoomSpriteMatrix *)&matrix, 1);
}

typedef struct RoomDriftView {
    char pad0[0x1A0];
    char transform[0x14];
    s32 base_x;
    s32 base_y;
    s32 base_z;
} RoomDriftView;

typedef struct RoomDriftRoot {
    char pad0[0x238];
    RoomDriftView *view;
} RoomDriftRoot;

typedef struct RoomDriftEffect {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0x2];
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    char padE[0x2];
    s16 timer;
    s16 duration;
    s16 half14;
} RoomDriftEffect;

typedef RoomFxSeed8 RoomDriftMatrixSeed;

s32 rand(void);

void RoomFx_UpdateDriftingEffect(void *arg0, void *arg1, RoomDriftEffect *effect) {
    register RoomDriftEffect *initial asm("$16");
    RoomDriftView *view;
    RoomDriftEffect *current;
    RoomDriftMatrixSeed seed;
    u16 out[4];
    register s32 random asm("$2");
    u16 timer;
    u16 x;
    u16 y;
    u16 z;
    u16 velocity_x;
    u16 velocity_y;
    u16 velocity_z;

    asm("" : "=r"(initial) : "0"(effect));
    view = (*(RoomDriftRoot **)func_800C2B50())->view;
    seed = s_EffectProjectSeed;
    ApplyMatrixSV(&view->transform, &seed, out);

    current = initial;
    if (initial->timer == 0) {
        initial->half14 = 0;
        initial->x = out[0] + view->base_x;
        initial->y = out[1] + view->base_y + 0x1E;
        initial->z = out[2] + view->base_z;
        initial->velocity_x = 0;
        initial->velocity_y = -8;
        initial->velocity_z = 0;
        random = rand();
        initial->duration = 0x96;
        initial->timer = random % 88 + 40;
    }

    x = current->x;
    velocity_x = current->velocity_x;
    velocity_y = current->velocity_y;
    velocity_z = current->velocity_z;
    current->x = x + velocity_x;
    y = current->y;
    z = current->z;
    current->y = y + velocity_y;
    current->z = z + velocity_z;
    random = rand();
    timer = current->timer - 4;
    current->timer = timer;
    current->duration += random % 30 + 50;
    if ((s16)timer < 0) {
        current->timer = 0;
    }
}
