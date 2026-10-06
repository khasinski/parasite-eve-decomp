/* MASPSX_FLAGS: --expand-div */
/*
 * The drop field effect set: a controller (init, no-op, hold timer) and
 * twelve init/draw/update triples, two of them empty.
 *
 * room_m174, room_m348 and room_m383 link the same 39 functions in this
 * order, with the same 0x70 bytes of read-only data; this unit is that
 * object, compiled into each of them. The sprite and ring records, the
 * spawn tables and the images live in each room's own data. The object
 * starts after the no-op that ends the room prefix and, in room_m174 and
 * room_m383, ends where the room library begins.
 */
#include "pe1/room_drop_field.h"
#include "pe1/field_script_context.h"
#include "pe1/room_spark.h"
#include "pe1/room_staged_motion.h"
#include "pe1/field_collision.h"
#include "pe1/gte_short_vector.h"
#include "pe1/gte.h"
#include "pe1/room_beam_pair.h"

#define RW16(o, off) (*(short *)((char *)(o) + (off)))
#define RW32(o, off) (*(int *)((char *)(o) + (off)))
#define RW8(o, off) (*(u8 *)((char *)(o) + (off)))
#define RWU16(o, off) (*(u16 *)((char *)(o) + (off)))

static const GteShortVector s_DropFieldTiltX = { 0x400, 0, 0, 0 };
static const GteShortVector s_DropFieldTiltY = { 0, 0x400, 0, 0 };
static const GteShortVector s_DropFieldNoTilt = { 0, 0, 0, 0 };
static const RoomFxVec4 s_DropFieldSpriteScale = { 0x400, 0x400, 0x400, 0 };
static const RoomFxVec4 s_DropFieldFlatScale = { 0x978, 0x978, 0x978, 0 };
static const RoomFxVec4 s_DropFieldModelScale = { 0x1190, 0x1190, 0x1190, 0 };
static const RoomFxVec4 s_DropFieldShadowScale = { 0xBE8, 0xBE8, 0xBE8, 0 };
static const GteShortVector s_EightParticleOffset = { 0, 0x28, -0x32, 0 };
static const GteShortVector s_EightParticleStepRight = { 0xA, 0, 0, 0 };
static const GteShortVector s_EightParticleStepLeft = { -0xA, 0, 0, 0 };





void RoomFx_DropFieldInit(RoomDropFieldEntity *ent, void *unused, RoomDropFieldState *state) {
    RoomDropFieldLink *link;
    RoomDropFieldMatrixWords *matrix;

    func_800C2B40(state);
    g_RoomDropFieldModel = func_8006E498(D_800B0E64, 0x118704);

    link = ent->link;
    state->link = link;
    matrix = (RoomDropFieldMatrixWords *)link->view;
    state->matrix0 = *matrix;
    matrix = (RoomDropFieldMatrixWords *)((char *)state->link->view + 0xA0);
    state->matrix1 = *matrix;
    state->asset = func_8006DC18(9);

    RW16(state, 0x4A) = 0;
    RW16(state, 0x4C) = 0;
    RW16(state, 0x48) = 0x96;
    RW16(state, 0x4E) = *func_800C2B28(0);
    RW32(state, 0x50) = *func_800C2B28(1);
    RW32(state, 0x54) = *func_800C2B28(2);
    RW32(state, 0x58) = *func_800C2B28(3);

    g_RoomDropFieldSpinSprite.code = 0x22;
    g_RoomDropFieldSpinSprite.mode = 0x30;
    g_RoomDropFieldDoubleSprite.code = 0x40;
    g_RoomDropFieldDoubleSprite.mode = 5;
    g_RoomDropFieldModelSprite.offset = 0x64;
    g_RoomDropFieldDropSprite.code = 6;
    g_RoomDropFieldSpinSprite.offset = 0;
    g_RoomDropFieldSpinSprite.depth = 0x80;
    g_RoomDropFieldSpinSprite.r = 0x80;
    g_RoomDropFieldSpinSprite.g = 0x80;
    g_RoomDropFieldSpinSprite.b = 0x80;
    g_RoomDropFieldSpinSprite.zero = 0;
    g_RoomDropFieldDoubleSprite.offset = 0;
    g_RoomDropFieldDoubleSprite.depth = 0x80;
    g_RoomDropFieldDoubleSprite.r = 0x80;
    g_RoomDropFieldDoubleSprite.g = 0x80;
    g_RoomDropFieldDoubleSprite.b = 0x80;
    g_RoomDropFieldDoubleSprite.zero = 0;
    g_RoomDropFieldMotionSprite.offset = 0;
    g_RoomDropFieldMotionSprite.depth = 0x80;
    g_RoomDropFieldMotionSprite.r = 0x80;
    g_RoomDropFieldMotionSprite.g = 0x80;
    g_RoomDropFieldMotionSprite.b = 0x80;
    g_RoomDropFieldMotionSprite.zero = 0;
    g_RoomDropFieldModelSprite.code = 0x68;
    g_RoomDropFieldModelSprite.mode = 7;
    g_RoomDropFieldModelSprite.depth = 0x80;
    g_RoomDropFieldModelSprite.r = 0x80;
    g_RoomDropFieldModelSprite.g = 0x80;
    g_RoomDropFieldModelSprite.b = 0x80;
    g_RoomDropFieldModelSprite.zero = 0;
    g_RoomDropFieldDropSprite.mode = 0x60;
    g_RoomDropFieldQuadSprite.code = 2;
    g_RoomDropFieldDropSprite.offset = 0;
    g_RoomDropFieldDropSprite.depth = 0x60;
    g_RoomDropFieldDropSprite.r = 0x80;
    g_RoomDropFieldDropSprite.g = 0x80;
    g_RoomDropFieldDropSprite.b = 0x80;
    g_RoomDropFieldDropSprite.zero = 0;
    g_RoomDropFieldQuadSprite.mode = 0x20;
    g_RoomDropFieldQuadSprite.offset = 0;
    g_RoomDropFieldQuadSprite.depth = 0x60;
    g_RoomDropFieldQuadSprite.r = 0x80;
    g_RoomDropFieldQuadSprite.g = 0x80;
    g_RoomDropFieldQuadSprite.b = 0x80;
    g_RoomDropFieldQuadSprite.zero = 0;
    g_RoomDropFieldParticleSprite.code = 0x68;
    g_RoomDropFieldParticleSprite.mode = 7;
    g_RoomDropFieldParticleSprite.offset = 0;
    g_RoomDropFieldParticleSprite.depth = 0x80;
    g_RoomDropFieldParticleSprite.zero = 0;
}




void RoomFx_DropFieldNop(void) {
}





void RoomFx_DropFieldTick(int arg0, char* arg1, char* arg2) {
    short* state = (short*)arg2;

    if (state[0x25] != 0) {
        state[0x25]--;
    }

    if (state[0x26] == 1) {
        int timer = state[0x25];

        state[0x26] = 0;
        if (timer == 0) {
            state[0x25] = *(unsigned short*)(arg2 + 0x48);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}




void RoomFx_InitStageSpawn(void) {
}




void RoomFx_DrawStageSpawn(void) {
}



void RoomFx_UpdateStageSpawn(void *effect, char *state, RoomFxSeed8 *seed) {
    char *owner = func_800C2B50();
    char *slot;

    if (*(s16 *)(state + 2) == 0) {
        slot = func_800C2B90(effect, 4, g_RoomDropFieldSpawnScript, g_RoomDropFieldSpawnData);
        if (slot != 0) {
            *(RoomFxSeed8 *)slot = *seed;
        }
        if (*(s16 *)(state + 2) == 0) {
            slot = func_800C2B90(effect, 3, g_RoomDropFieldSpawnScript, g_RoomDropFieldSpawnData);
            if (slot != 0) {
                *(RoomFxSeed8 *)slot = *seed;
            }
            if (*(s16 *)(state + 2) == 0) {
                slot = func_800C2B90(effect, 2, g_RoomDropFieldSpawnScript, g_RoomDropFieldSpawnData);
                if (slot != 0) {
                    *(s16 *)(slot + 0x14) = 1;
                    *(RoomFxSeed8 *)slot = *seed;
                }
            }
        }
    }
    if (*(s16 *)(state + 2) == 4) {
        slot = func_800C2B90(effect, 2, g_RoomDropFieldSpawnScript, g_RoomDropFieldSpawnData);
        if (slot != 0) {
            *(s16 *)(slot + 0x14) = 0;
            *(RoomFxSeed8 *)slot = *seed;
        }
    }
    if (*(s16 *)(state + 2) == 6) {
        slot = func_800C2B90(effect, 5, g_RoomDropFieldSpawnScript, g_RoomDropFieldSpawnData);
        if (slot != 0) {
            int i;

            *(RoomFxSeed8 *)slot = *seed;
            if (*(s16 *)(owner + 0x4E) == 0) {
                *(s16 *)(slot + 0xE8) = 8;
                *(s16 *)(slot + 0xEC) = 8;
                /* The original loop compares the signed halfword count as unsigned. */
                for (i = 0; i < (u32)*(s16 *)(slot + 0xE8); i++) {
                    *(s16 *)(slot + 0x98 + i * 2) = i << 9;
                }
            } else {
                *(s16 *)(slot + 0xE8) = 6;
                *(s16 *)(slot + 0xEC) = 6;
                for (i = 0; i < (u32)*(s16 *)(slot + 0xE8); i++) {
                    *(s16 *)(slot + 0x98 + i * 2) = i * 0x2A8;
                }
            }
        }
    }
    if (*(s16 *)(state + 2) >= 0x3D) {
        state[1] = 2;
    }
}





void RoomFx_InitSpinSprite(void* arg0, void* arg1, unsigned char* arg2) {
    *(s16*)(arg2 + 0x10) = 0x1418;
    *(s16*)(arg2 + 0x12) = 0xFF;
    *(s16*)(arg2 + 8) = 0;
    *(s16*)(arg2 + 0xA) = 0;
    *(s16*)(arg2 + 0xC) = 0;
}








void RoomFx_DrawSpinSprite(s32 arg0, s32 arg1, char *arg2) {
    RoomDropFieldSpinFrame stack;
    s16 *ptr;

    func_800C2EAC((u8)((char *)func_800C2B50())[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);
    RotMatrix(arg2 + 8, &stack.sp10);
    func_80071A44(&stack.sp40, 0, 0x10);

    {
        s32 x = *(s16 *)(arg2 + 0x10);
        stack.sp40.x = x;
    }
    {
        s32 y = *(s16 *)(arg2 + 0x10);
        stack.sp40.z = 0x1000;
        stack.sp40.y = y;
    }
    stack.sp30 = stack.sp40;
    ScaleMatrix(&stack.sp10, &stack.sp30);

    ptr = &g_RoomDropFieldSpinSprite.depth;
    *ptr = *(u16 *)(arg2 + 0x12);
    stack.sp24[0] = *(s16 *)(arg2 + 0);
    stack.sp24[1] = *(s16 *)(arg2 + 2);
    stack.sp24[2] = *(s16 *)(arg2 + 4);
    func_800C42A4((FieldGlowSprite *)(ptr - 5), (GteMatrix *)&stack.sp10, 1);
}




void RoomFx_UpdateSpinSprite(s32 arg0, char *arg1, char *arg2) {
    char *obj = arg2;
    u16 value;

    if (*(s16 *)(arg2 + 0x14) == 1) {
        *(u16 *)(arg2 + 0x10) += 0xC8;
    }

    if (*(s16 *)(arg2 + 0x14) == 0) {
        *(u16 *)(arg2 + 0x10) -= 0xC8;
    }

    value = *(u16 *)(obj + 0x12) - 0x10;
    *(u16 *)(obj + 0x12) = value;
    if ((s16)value < 0) {
        *(u16 *)(obj + 0x12) = 0;
    }

    if (*(s16 *)(arg1 + 2) >= 0x15) {
        arg1[1] = 2;
    }
}





void RoomFx_InitDoubleSprite(void* arg0, void* arg1, unsigned char* arg2) {
    *(s16*)(arg2 + 0x10) = 0xAD4;
    *(s16*)(arg2 + 0x12) = 0xFF;
    *(s16*)(arg2 + 8) = 0;
    *(s16*)(arg2 + 0xA) = 0;
    *(s16*)(arg2 + 0xC) = 0;
    *(s16*)(arg2 + 0x14) = 0;
}


/* This view starts at the alpha byte inside the room's sprite state. */
#define ROOM_SPRITE_GLOBALS ((RoomDoubleSpriteGlobals *)&g_RoomDropFieldDoubleSprite.code)


void RoomFx_DrawDoubleSprite(void *arg0, void *arg1, RoomDoubleSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxSeed8 seed;
    RoomFxVec4 scratch;
    RoomFxVec4 scale;
    RoomFxVec4 second_scale;
    char *owner;
    u16 *depth_slot;

    owner = func_800C2B50();
    seed = *(RoomFxSeed8 *)&s_DropFieldTiltX;
    func_800C2EAC((u8)owner[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    RotMatrix(&fx->seed, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratch = scale;
    ScaleMatrix(&matrix, &scratch);

    depth_slot = &ROOM_SPRITE_GLOBALS->depth;
    *depth_slot = fx->depth;
    ROOM_SPRITE_GLOBALS->alpha = fx->alpha * 2 + 0x40;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    func_800C42A4((FieldGlowSprite *)((char *)depth_slot - 0xA), &matrix, 1);

    RotMatrix(&seed, &matrix);
    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = fx->scale;
    second_scale.y = fx->scale;
    second_scale.z = 0x1000;
    scale = second_scale;
    ScaleMatrix(&matrix, &scale);

    *depth_slot = (s16)fx->depth >> 2;
    matrix.t[0] = fx->x;
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = fx->z;
    func_800C42A4((FieldGlowSprite *)((char *)depth_slot - 0xA), &matrix, 0);
}




void RoomFx_UpdateDoubleSprite(int arg0, char *arg1, char *arg2) {
    u16 value;

    *(u16 *)(arg2 + 0x10) = *(u16 *)(arg2 + 0x10) + 0xC8;
    *(u16 *)(arg2 + 0x12) = *(u16 *)(arg2 + 0x12) - 0x10;
    value = *(u16 *)(arg2 + 0x14) + 1;
    *(u16 *)(arg2 + 0x14) = value;
    if ((s16)value >= 8) {
        arg1[1] = 2;
    }

    if (*(s16 *)(arg2 + 0x12) < 0) {
        *(u16 *)(arg2 + 0x12) = 0;
    }
}





typedef struct RoomFxInitBounds {
    char pad0[0x10];
    s16 width;
    s16 height;
    s16 alpha;
    s16 depth;
} RoomFxInitBounds;



void RoomFx_InitRingPair(int unused0, int unused1,
                                  RoomFxInitBounds *bounds) {
    s16 *anchor;
    char *source;
    int mode;
    register int intensity asm("$19");
    int color1_0;
    register int color1_1 asm("$22");
    register int color1_2 asm("$20");
    register int extent1 asm("$18");
    RoomFxEmitterParams *params;
    RoomFxEmitterParams *secondary;
    int extent0;

    anchor = &g_RoomDropFieldRings[0].mode;
    params = (RoomFxEmitterParams *)(anchor - 6);

    bounds->width = 10;
    bounds->height = 10;
    bounds->depth = 20;
    bounds->alpha = 255;

    mode = 16;
    intensity = 128;
    color1_0 = 120;
    color1_1 = 100;
    color1_2 = 30;
    asm volatile("" ::: "memory");
    extent0 = 0x6A4;
    extent1 = 0x514;
    asm volatile("" ::: "memory");
    source = g_RoomDropFieldRingImages;

    *anchor = mode;
    g_RoomDropFieldRings[0].offset = 0;
    g_RoomDropFieldRings[0].intensity = intensity;
    g_RoomDropFieldRings[0].color1[0] = color1_0;
    g_RoomDropFieldRings[0].color1[1] = color1_1;
    g_RoomDropFieldRings[0].color1[2] = color1_2;
    g_RoomDropFieldRings[0].color0[0] = 0;
    g_RoomDropFieldRings[0].color0[1] = 0;
    g_RoomDropFieldRings[0].color0[2] = 0;
    g_RoomDropFieldRings[0].extent0 = extent0;
    g_RoomDropFieldRings[0].extent1 = extent1;
    params->source = source;
    func_800C4E50(params);

    params = (RoomFxEmitterParams *)(anchor + 6);
    extent0 = 0x226;
    asm volatile("" : "=r"(anchor) : "0"(anchor));
    source += 0x100;
    secondary = (RoomFxEmitterParams *)(anchor + 6);
    secondary->mode = mode;
    secondary->offset = 0;
    secondary->intensity = intensity;
    secondary->color1[0] = 0;
    secondary->color1[1] = 0;
    secondary->color1[2] = 0;
    secondary->color0[0] = color1_0;
    secondary->color0[1] = color1_1;
    secondary->color0[2] = color1_2;
    secondary->extent1 = extent0;
    secondary->extent0 = extent1;
    secondary->source = source;
    func_800C4E50(params);
}






void RoomFx_DrawRingPair(void *unused0, void *unused1,
                              RoomLayeredSpriteParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomSpriteMatrix saved_matrix;
    RoomFxVec4 scratch;
    RoomFxVec4 scale;
    RoomFxVec4 second_scale;
    u16 *depth_slot;

    seed0 = *(RoomFxSeed8 *)&s_DropFieldTiltX;
    seed1 = *(RoomFxSeed8 *)&s_DropFieldTiltY;
    func_800C3238(2);
    depth_slot = (u16 *)&g_RoomDropFieldRings[0].intensity;
    *depth_slot = fx->depth;
    g_RoomDropFieldRings[1].intensity = fx->depth;
    RotMatrix(&seed1, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = fx->scale;
    scratch = scale;
    ScaleMatrix(&matrix, &scratch);

    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    saved_matrix = matrix;
    func_800C4FC4((char *)depth_slot - 0x14, &matrix, 0);
    matrix = saved_matrix;
    func_800C4FC4(depth_slot + 2, &matrix, 0);

    RotMatrix(&seed0, &matrix);
    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = fx->scale * 2;
    second_scale.y = fx->scale * 2;
    second_scale.z = fx->scale * 2;
    scale = second_scale;
    ScaleMatrix(&matrix, &scale);

    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    saved_matrix = matrix;
    func_800C4FC4((char *)depth_slot - 0x14, &matrix, 0);
    matrix = saved_matrix;
    func_800C4FC4(depth_slot + 2, &matrix, 0);
}






void RoomFx_UpdateRingPair(void *unused, u8 *signal, char *state) {
    u16 h10;
    u16 h12;
    u16 h16;
    u16 h14;

    h10 = RWU16(state, 0x10);
    h12 = RWU16(state, 0x12);
    h16 = RWU16(state, 0x16);
    RWU16(state, 0x10) = h10 + 0x64;
    RWU16(state, 0x12) = h12 + h16;
    RWU16(state, 0x16) = RWU16(state, 0x16) + 0x32;
    h14 = RWU16(state, 0x14) - 0x10;
    RWU16(state, 0x14) = h14;
    if ((short)h14 < 0) {
        signal[1] = 2;
        RWU16(state, 0x14) = 0;
    }
}




void RoomFx_InitStagedMotion(void *unused0, void *unused1, char *state) {
    char *owner;
    unsigned int i;

    owner = func_800C2B50();
    RWU16(state, 0xEA) = 0x80;
    RW32(state, 0xE0) = 0xFFEC0000;
    RW32(state, 0xE4) = RW32((char *)owner, 0x58) << 8;

    for (i = 0; i < 8; i++) {
        RWU16(state, 0x88 + i * 2) = 0;
        RW32(state, 0xA8 + i * 4) = 0;
        RW32(state, 0x0C + i * 0x10) = RW16(state, 2) << 16;
        RW8(state + i, 0xC8) = 1;
        RWU16(state, 0xD0 + i * 2) = 0x80;
    }
}





typedef struct RoomStagedMotionRenderOwner {
    u8 pad00[0x44];
    u8 owner;                         /* 0x44: render owner id */
} RoomStagedMotionRenderOwner;

/* Emitter parameters; only the y origin selects the sprite variant. */
typedef struct RoomStagedMotionEmitter {
    s16 x;
    u16 y;
} RoomStagedMotionEmitter;

/* Rotation seed for RotMatrix; the halfword at 4 is the per-record
 * spin phase. */
typedef struct RoomStagedMotionSeed {
    u8 bytes[4];
    u16 phase;
    u16 pad06;
} RoomStagedMotionSeed;

/* Sprite packet body that follows the 4-byte packet base. */
typedef struct RoomStagedMotionPacket {
    u8 code;
    u8 mode;
    u8 pad02[4];
    u16 depth;
} RoomStagedMotionPacket;



/* Draws every staged motion record: kind 1 is a spinning sprite plus its
 * ground shadow at the frame-counter height, kinds 2..0xA add a flat
 * sprite 200 units up, and every kind >= 2 draws the model scaled by the
 * stage. The loop entry test is written as the counter compare so the
 * retail frame keeps its spare 8-byte slot. */
void RoomFx_DrawStagedMotion(void *unused,
                                     RoomStagedMotionEmitter *emitter,
                                     RoomStagedMotionState *state)
{
    RoomStagedMotionSeed seedA;
    RoomStagedMotionSeed seedB;
    RoomSpriteMatrix matrix;
    RoomSpriteMatrix saved;
    RoomFxVec4 scale;
    RoomFxVec4 scale2;
    RoomFxVec4 scale3;
    RoomStagedMotionRenderOwner *owner;
    RoomStagedMotionPacket *packet;
    u16 *flatDepth;
    u16 *modelDepth;
    int i;
    int variant;
    int handle;

    owner = (RoomStagedMotionRenderOwner *)func_800C2B50();
    seedA = *(RoomStagedMotionSeed *)&s_DropFieldNoTilt;
    seedB = *(RoomStagedMotionSeed *)&s_DropFieldTiltX;
    func_800C2EAC((u8)owner->owner);
    i = 0;
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);

    if ((unsigned int)i < state->count) {
        /* Both packet views are fixed before the loop: the sprite stage
         * then addresses the packet fields through the base register and
         * the flat stage through the depth pointer, as retail does. */
        packet = (RoomStagedMotionPacket *)&g_RoomDropFieldMotionSprite.code;
        flatDepth = &packet->depth;
        do {
            if (state->kind[i] == 1) {
                func_800C3238(2);
                seedA.phase = state->phase[i];
                RotMatrix(&seedA, &matrix);
                scale = s_DropFieldSpriteScale;
                ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                /* The int temporary keeps the remainder in full width. */
                variant = (s16)emitter->y;
                packet->code = (variant % 3) * 2 + 0x44;
                packet->mode = 5;
                packet->depth = state->size;
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = (state->record[i].y >> 16) + state->y;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                func_800C42A4((FieldGlowSprite *)&g_RoomDropFieldMotionSprite, &matrix, 1);
                packet->mode = 9;
                RotMatrix(&seedB, &matrix);
                scale2 = s_DropFieldSpriteScale;
                ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale2);
                packet->depth = (s16)state->size >> 2;
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = g_RoomFloorY->y;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                func_800C42A4((FieldGlowSprite *)&g_RoomDropFieldMotionSprite, &matrix, 0);
                packet->mode = 0xB;
            }
            if (state->kind[i] >= 2) {
                func_800C3238(2);
                if (state->kind[i] < 0xB) {
                    func_800C3098(0x10);
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
                    scale = s_DropFieldFlatScale;
                    ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                    *flatDepth = state->size;
                    *((u8 *)flatDepth - 6) = state->kind[i] * 2 + 0x3A;
                    *((u8 *)flatDepth - 5) = 5;
                    matrix.t[0] = (state->record[i].x >> 16) + state->x;
                    matrix.t[1] = (state->record[i].y >> 16) + state->y - 0xC8;
                    matrix.t[2] = (state->record[i].z >> 16) + state->z;
                    func_800C42A4((FieldGlowSprite *)((char *)flatDepth - 10), &matrix, 1);
                }
                func_800C3098(0x100);
                /* Taken here, not before the loop, so the depth pointer
                 * outranks the rematerialised packet base for $fp. */
                modelDepth = &g_RoomDropFieldSpinSprite.depth;
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
                scale = s_DropFieldModelScale;
                ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                *modelDepth = state->depth[i];
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = (state->record[i].y >> 16) + state->y - 0xC8;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                saved = matrix;
                func_800C42A4((FieldGlowSprite *)((char *)modelDepth - 10), &matrix, 1);
                matrix = saved;
                matrix.t[1] += 0xC8;
                memset(&scale3, 0, 0x10);
                scale3.x = state->kind[i] * 0x20 + 0x20C;
                scale3.y = state->kind[i] * 0x10;
                scale3.z = state->kind[i] * 0x20 + 0x20C;
                scale2 = scale3;
                ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale2);
                func_800C6D5C(g_RoomDropFieldModel, 0, 0);
                func_800C6EE8(-0x64);
                handle = (u16)func_80077A64(1, 1, 0x340, 0x100);
                func_800C6EC0(handle, (u16)func_80077AA4(0, 0x1DB));
                func_800C6ED8(1);
                func_800C6EF8(g_RoomDropFieldModel);
                func_800C6FA0(g_RoomDropFieldModel,
                              (u16)(state->depth[i] * 2));
                func_800C71E4(g_RoomDropFieldModel, &matrix);
                func_800C6F4C(g_RoomDropFieldModel);
            }
            i++;
        } while ((unsigned int)i < state->count);
    }
}






typedef struct RoomStagedMotionUpdateOwner {
    u8 pad00[0x4C];
    s16 hit;                          /* 0x4C: set when a record is in range */
    u8 pad4E[2];
    s32 radiusRate;                   /* 0x50: 4.12 radius growth factor */
    s32 fallRate;                     /* 0x54: fall step, in 8.8 */
} RoomStagedMotionUpdateOwner;

typedef struct RoomStagedMotionControl {
    u8 pad00;
    u8 state;                         /* 0x01: 2 once every record is spent */
} RoomStagedMotionControl;

typedef GteShortVector RoomStagedMotionPoint;



/* Advances every staged motion record: kind 1 spirals outwards and falls
 * until it leaves the floor polygon or reaches the frame-counter height,
 * spawning two child effects at that point; kinds 2.. count up to 0x14 and
 * fade their depth. Any live record within 200 units flags the owner. The
 * for loop keeps the entry compare on the counter so the retail frame keeps
 * its spare 8-byte slot and the counter wins $s6. */
void RoomFx_UpdateStagedMotion(void *entity,
                                       RoomStagedMotionControl *control,
                                       RoomStagedMotionState *state)
{
    RoomStagedMotionPoint radiusPoint;
    RoomStagedMotionPoint spawnPoint;
    RoomStagedMotionUpdateOwner *owner;
    char *spawn;
    int i;

    owner = (RoomStagedMotionUpdateOwner *)func_800C2B50();
    state->fallStep += owner->fallRate << 8;
    state->radiusStep += (state->radiusStep * owner->radiusRate) >> 12;
    for (i = 0; (unsigned int)i < state->count; i++) {
        if (state->kind[i] == 1) {
            state->phase[i] += 0x64;
            state->angle[i] += 0x14;
            state->radius[i] += state->radiusStep;
            state->record[i].x =
                ((rsin(state->angle[i]) * (state->radius[i] >> 12)) >> 12) << 16;
            state->record[i].y += state->fallStep;
            state->record[i].z =
                ((rcos(state->angle[i]) * (state->radius[i] >> 12)) >> 12) << 16;
            spawnPoint.x = (state->record[i].x >> 16) + state->x;
            spawnPoint.y = (state->record[i].y >> 16) + state->y;
            spawnPoint.z = (state->record[i].z >> 16) + state->z;
            if (Geo_PointInPoly(spawnPoint.x << 16, spawnPoint.z << 16,
                                (const PolygonVertex *)D_8009D248,
                                D_8009D1CC) == 0) {
                state->kind[i] = 0;
                state->remaining--;
                if (state->remaining == 0) {
                    control->state = 2;
                }
                spawn = func_800C2B90(entity, 6,
                                      g_RoomDropFieldSpawnScript,
                                      g_RoomDropFieldSpawnData);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
                spawn = func_800C2B90(entity, 6,
                                      g_RoomDropFieldSpawnScript,
                                      g_RoomDropFieldSpawnData);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
            }
            if (g_RoomFloorY->y < (state->record[i].y >> 16) + state->y) {
                state->kind[i] = 2;
                spawn = func_800C2B90(entity, 6,
                                      g_RoomDropFieldSpawnScript,
                                      g_RoomDropFieldSpawnData);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
                spawn = func_800C2B90(entity, 6,
                                      g_RoomDropFieldSpawnScript,
                                      g_RoomDropFieldSpawnData);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
            }
        }
        if (state->kind[i] >= 2) {
            state->kind[i]++;
            state->depth[i] -= 8;
            if ((s16)state->depth[i] < 0) {
                state->depth[i] = 0;
            }
            if (state->kind[i] >= 0xB && state->kind[i] == 0x14) {
                state->remaining--;
                if (state->remaining == 0) {
                    control->state = 2;
                }
            }
        }
        if (state->kind[i] != 0) {
            radiusPoint.x = (state->record[i].x >> 16) + state->x;
            radiusPoint.y = (state->record[i].y >> 16) + state->y;
            radiusPoint.z = (state->record[i].z >> 16) + state->z;
            if (func_800C6B90(&radiusPoint, 0xC8)) {
                owner->hit = 1;
            }
        }
    }
}






void RoomFx_InitModelSprite(int a, int b, RoomDropFieldDecal *c) {
    c->h10 = 0xB20;
    c->h8 = 0;
    c->hA = 0;
    c->hC = 0;
    c->h12 = 0xFF;
}








void RoomFx_DrawModelSprite(s32 arg0, s32 arg1, char *arg2) {
    RoomDropFieldSpinFrame stack;
    s16 *ptr;

    func_800C2EAC((u8)((char *)func_800C2B50())[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    RotMatrix(arg2 + 8, &stack.sp10);
    func_80071A44(&stack.sp40, 0, 0x10);

    {
        s32 x = *(s16 *)(arg2 + 0x10);
        stack.sp40.x = x;
    }
    {
        s32 y = *(s16 *)(arg2 + 0x10);
        stack.sp40.z = 0x1000;
        stack.sp40.y = y;
    }
    stack.sp30 = stack.sp40;
    ScaleMatrix(&stack.sp10, &stack.sp30);

    ptr = &g_RoomDropFieldModelSprite.depth;
    *ptr = *(u16 *)(arg2 + 0x12);
    stack.sp24[0] = *(s16 *)(arg2 + 0);
    stack.sp24[1] = *(s16 *)(arg2 + 2);
    stack.sp24[2] = *(s16 *)(arg2 + 4);
    func_800C42A4((FieldGlowSprite *)(ptr - 5), (GteMatrix *)&stack.sp10, 1);
}




void RoomFx_UpdateModelSprite(void *arg0, u8 *state, char *work) {
    *(u16 *)(work + 0x12) -= 8;
    *(u16 *)(work + 0x10) += 0x1E;
    *(u16 *)(work + 2) -= 0xF;

    if (*(s16 *)(work + 0x12) < 0) {
        *(u16 *)(work + 0x12) = 0;
        state[1] = 2;
    }
}




void RoomFx_InitIdleSlot(void) {
}




void RoomFx_DrawIdleSlot(void) {
}




void RoomFx_UpdateIdleSlot(void) {
}




/* Lays out the sixteen drops in a column above the anchor and starts their
 * sound. */
void RoomFx_InitDropField(s32 arg0, s32 arg1, RoomDropFieldDrops *state) {
    s32 *src;
    u32 i;

    src = func_800C2B50();
    state->anchor = *(RoomDropFieldQuad *)(src + 1);
    state->facing = *(RoomDropFieldQuad *)(src + 5);
    state->h180 = *func_800C2B10(1);
    state->h182 = *func_800C2B28(8);
    state->h184 = 0;

    for (i = 0; i < 16; i++) {
        state->pos[i].x = 0;
        state->pos[i].y = -0x12C;
        state->pos[i].z = i * 0x78;
        state->vel[i].x = 0;
        state->vel[i].y = 0;
        state->vel[i].z = 0;
        state->scale[i] = 0x800;
        state->height[i] = 0x60 - i * 3;
        state->flag[i] = 1;
    }

    func_800C6800(arg0, 0x56D, state->pos);
}




/* Draw every live drop below the release height: the drop sprite at its
 * world position (retired once it leaves the walkable polygon; touching
 * the player flags the actor) and its shadow on the floor. */
void RoomFx_DrawDropField(void *object, void *slot, RoomDropField *field) {
    GteMatrix matrix;
    GteMatrix saved;
    GteMatrix turn;
    RoomDropActor *actor = func_800C2B50();
    GteShortVector spin = s_DropFieldNoTilt;
    GteShortVector shadowSpin = s_DropFieldTiltX;
    GteShortVector rotation;
    GteShortVector point;
    GteVector scale;
    unsigned int i;

    func_800C2EAC(actor->shade);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);
    rotation.x = 0x20;
    rotation.y = field->yaw;
    rotation.z = 0;
    RotMatrix(&rotation, &turn);
    turn.t[0] = 0;
    turn.t[1] = 0;
    turn.t[2] = 0;
    for (i = 0; i < 16; i++) {
        if (field->live[i] == 1 && field->drops[i].z < -0x28A) {
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
            matrix.t[0] = field->drops[i].x;
            matrix.t[1] = field->drops[i].y;
            matrix.t[2] = field->drops[i].z;
            gte_CompMatrix(&turn, &matrix, &matrix);
            gte_CompMatrix(&field->matrix, &matrix, &matrix);
            RotMatrix(&spin, &matrix);
            scale = (GteVector){ field->size[i], field->size[i], field->size[i] };
            ScaleMatrix(&matrix, &scale);
            g_RoomDropFieldDropSprite.depth = field->depth[i];
            saved = matrix;
            point.x = matrix.t[0];
            point.y = matrix.t[1];
            point.z = matrix.t[2];
            if (Geo_PointInPoly(matrix.t[0] << 16, matrix.t[2] << 16, D_8009D248, D_8009D1CC) == 0) {
                field->live[i] = 0;
            }
            if (func_800C6B90(&point, 200) != 0) {
                actor->touched = 1;
            }
            func_800C42A4((FieldGlowSprite *)&g_RoomDropFieldDropSprite, &matrix, 1);
            matrix = saved;
            RotMatrix(&shadowSpin, &matrix);
            matrix.t[1] = g_RoomFloorY->y;
            {
                GteVector shadow = *(GteVector *)&s_DropFieldShadowScale;

                ScaleMatrix(&matrix, &shadow);
            }
            g_RoomDropFieldDropSprite.depth = field->depth[i] >> 1;
            func_800C42A4((FieldGlowSprite *)&g_RoomDropFieldDropSprite, &matrix, 0);
        }
    }
}







/* Sink every drop; when the script step reaches a drop and flag 7 is set,
 * retire it and, for the first one, spawn the splash at its world
 * position (drop offset turned by the field yaw and the field matrix). */
void RoomFx_UpdateDropField(void *object, RoomDropSlot *slot, RoomDropField *field) {
    GteMatrix matrix;
    GteMatrix turn;
    GteShortVector rotation;
    RoomDropPoint *splash;
    unsigned int i;

    for (i = 0; i < 16; i++) {
        field->drops[i].z -= 0x96;
        if (slot->step == field->first_step + i && *func_800C2B28(7) == 1) {
            field->live[i] = 0;
            if (++field->released == 1) {
                rotation.x = 0x20;
                rotation.y = field->yaw;
                rotation.z = 0;
                RotMatrix(&rotation, &turn);
                turn.t[0] = 0;
                turn.t[1] = 0;
                turn.t[2] = 0;
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
                matrix.t[0] = field->drops[i].x;
                matrix.t[1] = field->drops[i].y;
                matrix.t[2] = field->drops[i].z;
                gte_CompMatrix(&turn, &matrix, &matrix);
                gte_CompMatrix(&field->matrix, &matrix, &matrix);
                splash = func_800C2B90(object, 1, g_RoomDropFieldSpawnScript,
                                       g_RoomDropFieldSpawnData);
                if (splash != 0) {
                    splash->x = matrix.t[0];
                    splash->y = matrix.t[1];
                    splash->z = matrix.t[2];
                }
            }
        }
    }
    if (slot->step >= 0x79) {
        slot->command = 2;
    }
}





typedef struct RoomRenderWords8 {
    int words[8];
} RoomRenderWords8;


void RoomFx_InitBeamPair(void *unused0, void *unused1, void *output_arg) {
    char *work = output_arg;
    register short *anchor asm("$18");
    register int mode asm("$22");
    register int offset asm("$21");
    register int intensity asm("$20");
    register int primary_color asm("$17");
    register int blue asm("$23");
    register int extent asm("$19");
    register char *params asm("$4");
    register int white asm("$2");
    int primary_extent;
    char *root;

    root = (char *)func_800C2B50();
    PE1_COMPILER_LAUNDER(work);
    anchor = &g_RoomDropFieldRings[2].mode;
    params = (char *)anchor - 12;
    mode = 0x10;
    offset = -500;
    intensity = 0x80;
    primary_color = 250;
    root = *(char **)root;
    blue = 120;
    root = *(char **)(root + 0x238);
    extent = 1400;
    *(RoomRenderWords8 *)work = *(RoomRenderWords8 *)(root + 0xA0);
    RW16(work, 0x30) = 1200;
    RW16(work, 0x34) = 255;

    primary_extent = 3200;
    PE1_COMPILER_USE(primary_extent);
    work = g_RoomDropFieldBeamImages;
    *anchor = mode;
    g_RoomDropFieldRings[2].offset = offset;
    g_RoomDropFieldRings[2].intensity = intensity;
    g_RoomDropFieldRings[2].color1[0] = primary_color;
    g_RoomDropFieldRings[2].color1[1] = primary_color;
    g_RoomDropFieldRings[2].color1[2] = blue;
    g_RoomDropFieldRings[2].color0[0] = 0;
    g_RoomDropFieldRings[2].color0[1] = 0;
    g_RoomDropFieldRings[2].color0[2] = 0;
    g_RoomDropFieldRings[2].extent0 = primary_extent;
    g_RoomDropFieldRings[2].extent1 = extent;
    *(char **)params = work;
    func_800C4E50(params);

    params = (char *)anchor + 12;
    PE1_COMPILER_USE(params);
    white = 255;
    work += 0x100;
    g_RoomDropFieldRings[3].mode = mode;
    g_RoomDropFieldRings[3].offset = offset;
    g_RoomDropFieldRings[3].intensity = intensity;
    g_RoomDropFieldRings[3].color1[0] = white;
    g_RoomDropFieldRings[3].color1[1] = white;
    g_RoomDropFieldRings[3].color1[2] = white;
    g_RoomDropFieldRings[3].color0[0] = primary_color;
    g_RoomDropFieldRings[3].color0[1] = primary_color;
    g_RoomDropFieldRings[3].color0[2] = blue;
    g_RoomDropFieldRings[3].extent1 = 0;
    g_RoomDropFieldRings[3].extent0 = extent;
    *(char **)params = work;
    func_800C4E50(params);
}







/* Draw both beam halves at the script actor's anchor, offset 100 up and
 * 100 back, spun by the room rotation and scaled uniformly. */
void RoomFx_DrawBeamPair(void *object, void *slot, RoomBeamPair *beam) {
    GteMatrix matrix;
    GteMatrix second;
    RoomBeamPairActor **actor = func_800C2B50();
    GteShortVector spin = s_DropFieldNoTilt;
    GteVector scale;

    beam->matrix = *(GteMatrix *)((*actor)->model + 0xA0);
    g_RoomDropFieldRings[2].intensity = beam->brightness;
    g_RoomDropFieldRings[3].intensity = beam->brightness;
    func_800C3238(2);
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
    matrix.t[0] = 0;
    matrix.t[1] = 100;
    matrix.t[2] = -100;
    gte_CompMatrix(&beam->matrix, &matrix, &matrix);
    RotMatrix(&spin, &matrix);
    scale = (GteVector){ beam->scale, beam->scale, beam->scale };
    ScaleMatrix(&matrix, &scale);
    second = matrix;
    func_800C4FC4((RoomBeamSprite *)&g_RoomDropFieldRings[2], &matrix, 1);
    func_800C4FC4((RoomBeamSprite *)&g_RoomDropFieldRings[3], &second, 1);
}




void RoomFx_UpdateBeamPair(s32 arg0, char *arg1, char *arg2) {
    char *obj = arg2;
    u16 value;

    value = *(u16 *)(arg2 + 0x30) - 0x96;
    *(u16 *)(arg2 + 0x30) = value;
    if ((s16)value < 0x190) {
        *(u16 *)(arg2 + 0x30) = 0x190;
    }

    if (*(s16 *)(arg1 + 2) >= 8) {
        value = *(u16 *)(obj + 0x34) - 0x10;
        *(u16 *)(obj + 0x34) = value;
        if ((s16)value < 0) {
            *(u16 *)(obj + 0x34) = 0;
            arg1[1] = 2;
        }
    }
}






void RoomFx_InitQuadSprites(void *unused0, void *unused1, void *output_arg) {
    char *base = output_arg;
    int index;
    int red;
    int blue;
    int mid;
    register char *color asm("$20");
    register char *scale asm("$21");
    register char *motion asm("$18");
    char *clock;
    unsigned int speed;
    int value;

    clock = (char *)func_800C2B50();
    PE1_COMPILER_LAUNDER(base);
    *(RoomRenderWords8 *)base = *(RoomRenderWords8 *)(clock + 4);

    index = 0;
    red = 159;
    blue = 137;
    mid = 64;
    PE1_COMPILER_USE(index);
    PE1_COMPILER_USE(red);
    PE1_COMPILER_USE(blue);
    PE1_COMPILER_USE(mid);
    scale = base;
    PE1_COMPILER_LAUNDER(scale);
    color = scale;
    motion = scale;

    do {
        speed = (int)rand() % 40 + 10;
        rand();

        value = -200;
        RW16(motion, 0x32) = value;
        RW16(motion, 0x34) = value;
        value = -(speed >> 1);
        speed <<= 6;
        RW16(motion, 0x50) = value;
        RW16(motion, 0x52) = value;
        value = 160;
        RW16(motion, 0x30) = 0;
        RW16(scale, 0x78) = value;
        RW16(scale, 0x70) = speed;

        if (index == 1) goto color1;
        if (index == 0) goto color0;
        if (index == 2) goto color2;
        if (index == 3) goto color3;
        color += 4;
        goto advance_scale;

color0:
        color[0x20] = red;
        color[0x21] = 138;
        color[0x22] = blue;
        goto advance_color;
color1:
        base[0x24] = mid;
        base[0x25] = 76;
        base[0x26] = 6;
        goto advance_color;
color2:
        base[0x28] = 18;
        base[0x29] = 32;
        base[0x2A] = mid;
        goto advance_color;
color3:
        base[0x2C] = red;
        base[0x2D] = 98;
        base[0x2E] = blue;
advance_color:
        color += 4;
advance_scale:
        scale += 2;
        motion += 8;
        index++;
    } while ((unsigned int)index < 4);
}





typedef struct RoomFourSpriteColor {
    u8 r, g, b, pad;
} RoomFourSpriteColor;

typedef struct RoomFourSpritePosition {
    s16 x, y, z, pad;
} RoomFourSpritePosition;

/* Per-room sprite set: the shared rotation matrix followed by the four
 * sprites' colors, positions, scales and packet depths. */
typedef struct RoomFourSpriteState {
    RoomSpriteMatrix rotation;            /* 0x00 */
    RoomFourSpriteColor color[4];         /* 0x20 */
    RoomFourSpritePosition position[4];   /* 0x30 */
    u8 reserved50[0x20];                  /* 0x50 */
    s16 scale[4];                         /* 0x70 */
    u16 depth[4];                         /* 0x78 */
} RoomFourSpriteState;

typedef struct RoomFourSpriteStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomFourSpriteStack;



/* Draws four scaled sprites. The state begins with the shared rotation
 * matrix; each sprite reads its position, scale, depth and color from the
 * arrays that follow it. Retail marks this function handwritten; only its
 * COP2 windows remain ASM. */
void RoomFx_DrawQuadSprites(void *unused0, void *unused1,
                                    RoomFourSpriteState *state) {
    RoomFourSpriteStack stack;
    char *clock;
    unsigned int i;
    /* Retail keeps the matrix base in s3 and the second column in s7; the
     * three sprite-array cursors are strength-reduced copies the compiler
     * places in s0..s2 after them. */
    register RoomSpriteMatrix *matrix asm("$19");
    register short *column1 asm("$23");
    /* One scratch pointer serves the last column and the translation, so
     * the loop optimizer does not hoist either address. */
    unsigned short *work;
    unsigned short *depthSlot;

    clock = func_800C2B50();
    stack.seed = *(RoomFxSeed8 *)&s_DropFieldNoTilt;
    i = 0;
    matrix = &stack.matrix;
    column1 = &stack.matrix.m[0][1];
    depthSlot = &g_RoomDropFieldQuadSprite.depth;
    func_800C2EAC((u8)clock[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);

    for (i = 0; i < 4; i++) {
        stack.matrix.m[2][2] = 0x1000;
        stack.matrix.m[1][1] = 0x1000;
        stack.matrix.m[0][0] = 0x1000;
        stack.matrix.t[2] = 0;
        stack.matrix.t[1] = 0;
        stack.matrix.t[0] = 0;
        stack.matrix.m[2][1] = 0;
        stack.matrix.m[2][0] = 0;
        stack.matrix.m[1][2] = 0;
        stack.matrix.m[1][0] = 0;
        stack.matrix.m[0][2] = 0;
        stack.matrix.m[0][1] = 0;
        stack.matrix.t[0] = state->position[i].x;
        stack.matrix.t[1] = state->position[i].y;
        stack.matrix.t[2] = state->position[i].z;

        gte_ldrotmatrix(&state->rotation);
        gte_ldrtir12_matrix_column(matrix);
        gte_stir123_column(matrix);
        gte_ldrtir12_matrix_column(column1);
        gte_stir123_column(column1);
        work = (unsigned short *)&stack.matrix.m[0][2];
        gte_ldrtir12_matrix_column(work);
        gte_stir123_column(work);
        gte_ldtransmatrix(&state->rotation);
        work = (unsigned short *)stack.matrix.t;
        gte_ldv0_word3(work);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        gte_swc2_9_0(work);
        gte_swc2_10_4(work);
        gte_swc2_11_8(work);

        RotMatrix(&stack.seed, matrix);
        memset(&stack.sourceScale, 0, 0x10);
        stack.sourceScale.x = state->scale[i];
        stack.sourceScale.y = state->scale[i];
        stack.sourceScale.z = state->scale[i];
        stack.scale = stack.sourceScale;
        ScaleMatrix(matrix, &stack.scale);
        *depthSlot = state->depth[i];
        ((u8 *)depthSlot)[-0xA] = state->color[i].r;
        ((u8 *)depthSlot)[-0x9] = state->color[i].g;
        ((u8 *)depthSlot)[-0x8] = state->color[i].b;
        func_800C42A4((FieldGlowSprite *)((char *)depthSlot - 0xA), matrix, 1);
    }
}






typedef struct RoomDropFieldQuadStep {
    char pad00[0x30];
    unsigned short h30;
    unsigned short h32;
    unsigned short h34;
    char pad36[0x1A];
    unsigned short h50;
    unsigned short h52;
    unsigned short h54;
} RoomDropFieldQuadStep;

void RoomFx_UpdateQuadSprites(void *unused, unsigned char *state, RoomDropFieldQuadStep *p) {
    unsigned int i;
    char *base = (char *)p;

    for (i = 0; i < 4; i++) {
        char *rec = base + i * 8;
        char *timer = base + i * 2;

        RWU16(rec, 0x30) += RWU16(rec, 0x50);
        RWU16(rec, 0x32) += RWU16(rec, 0x52);
        RWU16(rec, 0x34) += RWU16(rec, 0x54);
        RWU16(timer, 0x78) -= 8;
        if ((short)RWU16(timer, 0x78) < 0) {
            RWU16(timer, 0x78) = 0;
        }
    }

    if (*(short *)(state + 2) >= 0x3D) {
        state[1] = 2;
    }
}




void RoomFx_InitIdleSlot2(void) {
}




void RoomFx_DrawIdleSlot2(void) {
}




void RoomFx_UpdateIdleSlot2(void) {
}




void RoomFx_InitEightParticles(void* arg0, void* arg1, unsigned char* arg2) {
    unsigned int i;

    *(int*)arg2 = 0;

    for (i = 0; i < 8; i++) {
        *(arg2 + i + 0xB4) = 0;
    }
}







void RoomFx_DrawEightParticles(
    void *unused0, void *unused1, char *stateArg) {
    char *state;
    unsigned int i;
    int unitScale;
    u16 *depthOut;
    char *packet;
    register char *scalarCursor asm("$16");
    register char *positionCursor asm("$17");
    RoomSpriteMatrix matrix;
    int scaleArg[4];
    volatile int sourceScale[4];
    RoomSpriteMatrix *matrixArg;
    register int scale2 asm("$6");
    register int scale3 asm("$7");
    char *context;
    int secondScale;

    state = stateArg;
    context = func_800C2B50();
    i = 0;
    unitScale = 0x1000;
    depthOut = &g_RoomDropFieldParticleSprite.depth;
    packet = (char *)depthOut - 10;
    asm volatile("" ::: "memory");
    scalarCursor = state;
    positionCursor = scalarCursor;

    func_800C2EAC((u8)context[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    do {
        if (*(state + i + 0xB4) != 0) {
            matrix.m[2][2] = unitScale;
            matrix.m[1][1] = unitScale;
            matrix.m[0][0] = unitScale;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            memset((void *)sourceScale, 0, sizeof(sourceScale));
            sourceScale[0] = *(s16 *)(scalarCursor + 0x84);
            matrixArg = &matrix;
            asm volatile("" : "=r"(matrixArg) : "0"(matrixArg));
            secondScale = *(s16 *)(scalarCursor + 0x84);
            sourceScale[2] = unitScale;
            sourceScale[1] = secondScale;
            scaleArg[0] = sourceScale[0];
            scaleArg[1] = sourceScale[1];
            scale2 = sourceScale[2];
            scale3 = sourceScale[3];
            scaleArg[2] = scale2;
            scaleArg[3] = scale3;
            asm volatile("" ::: "$5");
            ScaleMatrix(matrixArg, (const GteVector *)scaleArg);

            *depthOut = *(u16 *)(scalarCursor + 0x94);
            matrix.t[0] = *(s16 *)(positionCursor + 4);
            matrix.t[1] = *(s16 *)(positionCursor + 6);
            matrix.t[2] = *(s16 *)(positionCursor + 8);
            func_800C3134(g_RoomDropFieldParticleFrames,
                          *(s16 *)(scalarCursor + 0xA4), packet);
            func_800C42A4((FieldGlowSprite *)packet, &matrix, 1);
        }
        positionCursor += 8;
        i++;
        scalarCursor += 2;
    } while (i < 8);
}








void RoomFx_UpdateEightParticles(void *unused,
                               RoomEightParticleControl *control,
                               RoomEightParticleState *stateArg) {
    RoomEightParticleControl *clock;
    register RoomEightParticleState *state asm("$19");
    unsigned int i;
    register char *positionCursor asm("$16");
    register char *scalarCursor asm("$17");
    RoomEightParticleContext *context;
    char *matrix;
    char *activeCursor;
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomFxSeed8 seed2;
    RoomFxSeed8 transformed0;
    RoomFxSeed8 transformed1;
    RoomFxSeed8 transformed2;
    RoomEightParticleContext *contextResult;
    char *matrixBase;
    int frame;
    s16 slot;
    int random;

    clock = control;
    state = stateArg;
    contextResult = func_800C2B50();
    i = 0;
    scalarCursor = (char *)state;
    context = contextResult;
    matrixBase = *(char **)(context->root + 0x238);
    positionCursor = (char *)state;
    seed0 = *(RoomFxSeed8 *)&s_EightParticleOffset;
    seed1 = *(RoomFxSeed8 *)&s_EightParticleStepRight;
    seed2 = *(RoomFxSeed8 *)&s_EightParticleStepLeft;
    matrix = matrixBase + 0xA0;

    do {
        activeCursor = (char *)state + i;
        if (activeCursor[0xB4] == 0) {
            frame = clock->frame;
            slot = frame % 8;
            if (slot == i && frame < 90) {
                activeCursor[0xB4] = 1;
                state->activeCount++;
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed0,
                              (GteShortVector *)&transformed0);
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed1,
                              (GteShortVector *)&transformed1);
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed2,
                              (GteShortVector *)&transformed2);

                *(u16 *)(positionCursor + 4) =
                    *(u16 *)&transformed0.bytes[0] + context->baseX;
                *(u16 *)(positionCursor + 6) =
                    *(u16 *)&transformed0.bytes[2] + context->baseY;
                *(u16 *)(positionCursor + 8) =
                    *(u16 *)&transformed0.bytes[4] + context->baseZ;

                random = rand();
                if ((random & 1) == 0) {
                    *(RoomFxSeed8 *)(positionCursor + 0x44) = transformed2;
                } else {
                    *(RoomFxSeed8 *)(positionCursor + 0x44) = transformed1;
                }
                *(u16 *)(scalarCursor + 0x84) = 0x800;
                *(s16 *)(scalarCursor + 0x94) = 0x80;
                *(u16 *)(scalarCursor + 0xA4) = 0;
            }
        } else {
            int randomValue;
            int yValue;
            int zVelocity;
            int zPosition;
            int zValue;

            *(u16 *)(positionCursor + 4) +=
                *(u16 *)(positionCursor + 0x44);
            randomValue = rand();
            randomValue %= 20;
            zPosition = *(u16 *)(positionCursor + 8);
            zVelocity = *(u16 *)(positionCursor + 0x48);
            asm("" : "=r"(zPosition), "=r"(zVelocity)
                   : "0"(zPosition), "1"(zVelocity));
            yValue = *(u16 *)(positionCursor + 6);
            yValue -= 20;
            zValue = zPosition + zVelocity;
            asm("" : "=r"(yValue), "=r"(zValue)
                   : "0"(yValue), "1"(zValue));
            *(u16 *)(positionCursor + 8) = zValue;
            yValue -= randomValue;
            *(u16 *)(positionCursor + 6) = yValue;
            *(s16 *)(scalarCursor + 0x94) -= 8;
            (*(u16 *)(scalarCursor + 0xA4))++;
            if (*(s16 *)(scalarCursor + 0x94) <= 0) {
                *(s16 *)(scalarCursor + 0x94) = 0;
                activeCursor[0xB4] = 0;
                state->activeCount--;
            }
        }

        scalarCursor += 2;
        i++;
        positionCursor += 8;
    } while (i < 8);

    if (state->activeCount == 0 && clock->frame >= 91) {
        clock->state = 2;
    }
}



