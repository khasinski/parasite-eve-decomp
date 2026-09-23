#include "common.h"
#include "pe1/room_fx.h"

extern RoomFxSeed8 ROOM_LAYERED_SPRITE_SEED0;
extern RoomFxSeed8 ROOM_LAYERED_SPRITE_SEED1;
extern s16 ROOM_LAYERED_SPRITE_PRIMARY_DEPTH;
extern u16 ROOM_LAYERED_SPRITE_SECONDARY_DEPTH;

void func_800C3238(s32);
void func_800794C4(RoomFxSeed8 *, RoomSpriteMatrix *);
void func_80071A44(void *, s32, s32);
void Gte_ScaleMatrix(RoomSpriteMatrix *, RoomFxVec4 *);
void func_800C4FC4(void *, RoomSpriteMatrix *, s32);

void ROOM_LAYERED_SPRITE_NAME(void *unused0, void *unused1,
                              RoomLayeredSpriteParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomSpriteMatrix saved_matrix;
    RoomFxVec4 scratch;
    RoomFxVec4 scale;
    RoomFxVec4 second_scale;
    u16 *depth_slot;

    seed0 = ROOM_LAYERED_SPRITE_SEED0;
    seed1 = ROOM_LAYERED_SPRITE_SEED1;
    func_800C3238(2);
    depth_slot = (u16 *)&ROOM_LAYERED_SPRITE_PRIMARY_DEPTH;
    *depth_slot = fx->depth;
    ROOM_LAYERED_SPRITE_SECONDARY_DEPTH = fx->depth;
    func_800794C4(&seed1, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = fx->scale;
    scratch = scale;
    Gte_ScaleMatrix(&matrix, &scratch);

    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    saved_matrix = matrix;
    func_800C4FC4((char *)depth_slot - 0x14, &matrix, 0);
    matrix = saved_matrix;
    func_800C4FC4((char *)depth_slot + 4, &matrix, 0);

    func_800794C4(&seed0, &matrix);
    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = fx->scale * 2;
    second_scale.y = fx->scale * 2;
    second_scale.z = fx->scale * 2;
    scale = second_scale;
    Gte_ScaleMatrix(&matrix, &scale);

    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    saved_matrix = matrix;
    func_800C4FC4((char *)depth_slot - 0x14, &matrix, 0);
    matrix = saved_matrix;
    func_800C4FC4((char *)depth_slot + 4, &matrix, 0);
}

#undef ROOM_LAYERED_SPRITE_NAME
#undef ROOM_LAYERED_SPRITE_SEED0
#undef ROOM_LAYERED_SPRITE_SEED1
#undef ROOM_LAYERED_SPRITE_PRIMARY_DEPTH
#undef ROOM_LAYERED_SPRITE_SECONDARY_DEPTH
