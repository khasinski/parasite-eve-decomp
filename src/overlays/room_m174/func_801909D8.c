#include "pe1/room_fx.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern u16 D_801973F2;

char *func_800C2B50(void);
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, RoomSpriteMatrix *matrix, s32 arg2);

void func_801909D8(void *arg0, void *arg1, RoomSeededSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratch_scale;
    RoomFxVec4 scale;
    char *owner;
    u16 *depth_slot;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);
    func_800794C4(&fx->seed, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratch_scale = scale;
    func_80078CC4(&matrix, &scratch_scale);

    depth_slot = &D_801973F2;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 1);
}
