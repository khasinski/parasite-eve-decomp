#include "common.h"
#include "pe1/room_fx.h"

void func_80190B44(void* arg0, void* arg1, unsigned char* arg2) {
    *(s16*)(arg2 + 0x10) = 0xAD4;
    *(s16*)(arg2 + 0x12) = 0xFF;
    *(s16*)(arg2 + 8) = 0;
    *(s16*)(arg2 + 0xA) = 0;
    *(s16*)(arg2 + 0xC) = 0;
    *(s16*)(arg2 + 0x14) = 0;
}

extern RoomFxSeed8 D_8018F00C;
extern u8 D_80197414;
extern s16 D_800942EC;

/* This view starts at the alpha byte inside the room's sprite state. */
#define ROOM_SPRITE_GLOBALS ((RoomDoubleSpriteGlobals *)&D_80197414)

char *func_800C2B50(void);
void func_80071A44(void *, s32, s32);
void func_800794C4(RoomFxSeed8 *, RoomSpriteMatrix *);
void Gte_ScaleMatrix(RoomSpriteMatrix *, RoomFxVec4 *);
void func_800C2EAC(u8);
void func_800C2FF0(s32, s32);
void func_800C3098(s32);
void func_800C3238(s32);
void func_800C42A4(void *, RoomSpriteMatrix *, s32);

void func_80190B68(void *arg0, void *arg1, RoomDoubleSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxSeed8 seed;
    RoomFxVec4 scratch;
    RoomFxVec4 scale;
    RoomFxVec4 second_scale;
    char *owner;
    u16 *depth_slot;

    owner = func_800C2B50();
    seed = D_8018F00C;
    func_800C2EAC(owner[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800794C4(&fx->seed, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratch = scale;
    Gte_ScaleMatrix(&matrix, &scratch);

    depth_slot = &ROOM_SPRITE_GLOBALS->depth;
    *depth_slot = fx->depth;
    ROOM_SPRITE_GLOBALS->alpha = fx->alpha * 2 + 0x40;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 1);

    func_800794C4(&seed, &matrix);
    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = fx->scale;
    second_scale.y = fx->scale;
    second_scale.z = 0x1000;
    scale = second_scale;
    Gte_ScaleMatrix(&matrix, &scale);

    *depth_slot = (s16)fx->depth >> 2;
    matrix.t[0] = fx->x;
    matrix.t[1] = D_800942EC;
    matrix.t[2] = fx->z;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}
