#include "common.h"
#include "pe1/field_script_context.h"
#include "pe1/room_fx.h"
#include "pe1/room_spark.h"
#include "pe1/room_staged_motion.h"
#ifndef ROOMLIB_DRAW_STAGED_MOTION_FUNC
#error "ROOMLIB_DRAW_STAGED_MOTION_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_STAGED_MOTION_PACKET_BASE
#define ROOMLIB_STAGED_MOTION_PACKET_BASE D_801974D0
#endif
#ifndef ROOMLIB_STAGED_MOTION_PACKET
#define ROOMLIB_STAGED_MOTION_PACKET D_801974D4
#endif
#ifndef ROOMLIB_STAGED_MOTION_MODEL_DEPTH
#define ROOMLIB_STAGED_MOTION_MODEL_DEPTH D_801973F2
#endif
#ifndef ROOMLIB_STAGED_MOTION_MODEL
#define ROOMLIB_STAGED_MOTION_MODEL D_80197404
#endif
#ifndef ROOMLIB_STAGED_MOTION_SEED_A
#define ROOMLIB_STAGED_MOTION_SEED_A D_8018F01C
#endif
#ifndef ROOMLIB_STAGED_MOTION_SEED_B
#define ROOMLIB_STAGED_MOTION_SEED_B D_8018F00C
#endif
#ifndef ROOMLIB_STAGED_MOTION_SPRITE_SCALE
#define ROOMLIB_STAGED_MOTION_SPRITE_SCALE D_8018F024
#endif
#ifndef ROOMLIB_STAGED_MOTION_FLAT_SCALE
#define ROOMLIB_STAGED_MOTION_FLAT_SCALE D_8018F034
#endif
#ifndef ROOMLIB_STAGED_MOTION_MODEL_SCALE
#define ROOMLIB_STAGED_MOTION_MODEL_SCALE D_8018F044
#endif

typedef struct RoomStagedMotionRenderOwner {
    u8 pad00[0x44];
    u8 owner;                         /* 0x44: render owner id */
} RoomStagedMotionRenderOwner;

/* Emitter parameters; only the y origin selects the sprite variant. */
typedef struct RoomStagedMotionEmitter {
    s16 x;
    u16 y;
} RoomStagedMotionEmitter;

/* Rotation seed for func_800794C4; the halfword at 4 is the per-record
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

void func_800C2EAC(u8 owner);
void func_800C2FF0(s32 width, s32 height);
void func_800C3098(s32 depth);
void func_800C3238(s32 mode);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, s32 mode);
void func_800794C4(void *seed, RoomSpriteMatrix *matrix);
void func_800C6D5C(void *model, int arg1, int arg2);
void func_800C6EE8(int arg0);
int func_80077A64(int arg0, int arg1, int arg2, int arg3);
void func_800C6EC0(int arg0, int arg1);
void func_800C6ED8(int arg0);
void func_800C6EF8(void *model);
void func_800C6FA0(void *model, int depth);
void func_800C71E4(void *model, RoomSpriteMatrix *matrix);
void func_800C6F4C(void *model);
void *memset(void *dst, int value, int size);

extern RoomStagedMotionSeed ROOMLIB_STAGED_MOTION_SEED_A;
extern RoomStagedMotionSeed ROOMLIB_STAGED_MOTION_SEED_B;
extern RoomFxVec4 ROOMLIB_STAGED_MOTION_SPRITE_SCALE;
extern RoomFxVec4 ROOMLIB_STAGED_MOTION_FLAT_SCALE;
extern RoomFxVec4 ROOMLIB_STAGED_MOTION_MODEL_SCALE;
extern u8 ROOMLIB_STAGED_MOTION_PACKET_BASE[];
extern RoomStagedMotionPacket ROOMLIB_STAGED_MOTION_PACKET;
extern u16 ROOMLIB_STAGED_MOTION_MODEL_DEPTH;
extern void *ROOMLIB_STAGED_MOTION_MODEL;

/* Draws every staged motion record: kind 1 is a spinning sprite plus its
 * ground shadow at the frame-counter height, kinds 2..0xA add a flat
 * sprite 200 units up, and every kind >= 2 draws the model scaled by the
 * stage. The loop entry test is written as the counter compare so the
 * retail frame keeps its spare 8-byte slot. */
void ROOMLIB_DRAW_STAGED_MOTION_FUNC(void *unused,
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
    seedA = ROOMLIB_STAGED_MOTION_SEED_A;
    seedB = ROOMLIB_STAGED_MOTION_SEED_B;
    func_800C2EAC(owner->owner);
    i = 0;
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);

    if ((unsigned int)i < state->count) {
        /* Both packet views are fixed before the loop: the sprite stage
         * then addresses the packet fields through the base register and
         * the flat stage through the depth pointer, as retail does. */
        packet = &ROOMLIB_STAGED_MOTION_PACKET;
        flatDepth = &packet->depth;
        do {
            if (state->kind[i] == 1) {
                func_800C3238(2);
                seedA.phase = state->phase[i];
                func_800794C4(&seedA, &matrix);
                scale = ROOMLIB_STAGED_MOTION_SPRITE_SCALE;
                Gte_ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                /* The int temporary keeps the remainder in full width. */
                variant = (s16)emitter->y;
                packet->code = (variant % 3) * 2 + 0x44;
                packet->mode = 5;
                packet->depth = state->size;
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = (state->record[i].y >> 16) + state->y;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                func_800C42A4(ROOMLIB_STAGED_MOTION_PACKET_BASE, &matrix, 1);
                packet->mode = 9;
                func_800794C4(&seedB, &matrix);
                scale2 = ROOMLIB_STAGED_MOTION_SPRITE_SCALE;
                Gte_ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale2);
                packet->depth = (s16)state->size >> 2;
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = D_800942EC.count;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                func_800C42A4(ROOMLIB_STAGED_MOTION_PACKET_BASE, &matrix, 0);
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
                    scale = ROOMLIB_STAGED_MOTION_FLAT_SCALE;
                    Gte_ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                    *flatDepth = state->size;
                    *((u8 *)flatDepth - 6) = state->kind[i] * 2 + 0x3A;
                    *((u8 *)flatDepth - 5) = 5;
                    matrix.t[0] = (state->record[i].x >> 16) + state->x;
                    matrix.t[1] = (state->record[i].y >> 16) + state->y - 0xC8;
                    matrix.t[2] = (state->record[i].z >> 16) + state->z;
                    func_800C42A4((char *)flatDepth - 10, &matrix, 1);
                }
                func_800C3098(0x100);
                /* Taken here, not before the loop, so the depth pointer
                 * outranks the rematerialised packet base for $fp. */
                modelDepth = &ROOMLIB_STAGED_MOTION_MODEL_DEPTH;
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
                scale = ROOMLIB_STAGED_MOTION_MODEL_SCALE;
                Gte_ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale);
                *modelDepth = state->depth[i];
                matrix.t[0] = (state->record[i].x >> 16) + state->x;
                matrix.t[1] = (state->record[i].y >> 16) + state->y - 0xC8;
                matrix.t[2] = (state->record[i].z >> 16) + state->z;
                saved = matrix;
                func_800C42A4((char *)modelDepth - 10, &matrix, 1);
                matrix = saved;
                matrix.t[1] += 0xC8;
                memset(&scale3, 0, 0x10);
                scale3.x = state->kind[i] * 0x20 + 0x20C;
                scale3.y = state->kind[i] * 0x10;
                scale3.z = state->kind[i] * 0x20 + 0x20C;
                scale2 = scale3;
                Gte_ScaleMatrix((GteMatrix *)&matrix, (GteVector *)&scale2);
                func_800C6D5C(ROOMLIB_STAGED_MOTION_MODEL, 0, 0);
                func_800C6EE8(-0x64);
                handle = (u16)func_80077A64(1, 1, 0x340, 0x100);
                func_800C6EC0(handle, (u16)func_80077AA4(0, 0x1DB));
                func_800C6ED8(1);
                func_800C6EF8(ROOMLIB_STAGED_MOTION_MODEL);
                func_800C6FA0(ROOMLIB_STAGED_MOTION_MODEL,
                              (u16)(state->depth[i] * 2));
                func_800C71E4(ROOMLIB_STAGED_MOTION_MODEL, &matrix);
                func_800C6F4C(ROOMLIB_STAGED_MOTION_MODEL);
            }
            i++;
        } while ((unsigned int)i < state->count);
    }
}

#undef ROOMLIB_DRAW_STAGED_MOTION_FUNC
#undef ROOMLIB_STAGED_MOTION_PACKET_BASE
#undef ROOMLIB_STAGED_MOTION_PACKET
#undef ROOMLIB_STAGED_MOTION_MODEL_DEPTH
#undef ROOMLIB_STAGED_MOTION_MODEL
#undef ROOMLIB_STAGED_MOTION_SEED_A
#undef ROOMLIB_STAGED_MOTION_SEED_B
#undef ROOMLIB_STAGED_MOTION_SPRITE_SCALE
#undef ROOMLIB_STAGED_MOTION_FLAT_SCALE
#undef ROOMLIB_STAGED_MOTION_MODEL_SCALE
