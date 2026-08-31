#include "common.h"
#include "pe1/room_fx.h"

extern void *D_80190194;

char *func_800C2B50(void);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C6D5C(void *arg0, s32 arg1, s32 arg2);
s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80077AA4(s32 arg0, s32 arg1);
void func_800C6EC0(s32 arg0, s32 arg1);
void func_800C6ED8(s32 arg0);
void func_800C6EF8(void *arg0);
void func_800C7098(void *arg0, s32 arg1, s32 arg2, s32 arg3);
void func_800C6FA0(void *arg0, s32 arg1);
void func_800C71E4(void *arg0, RoomSpriteMatrix *matrix);
void func_800C6F4C(void *arg0);

void func_8018F92C(void *arg0, void *arg1, RoomSeededSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratch_scale;
    RoomFxVec4 scale;
    char *state;

    state = func_800C2B50();
    func_800794C4(&fx->seed, &matrix);

    matrix.t[0] = *(s16 *)(*(char **)((char *)arg0 + 8) + 0x2A);
    matrix.t[1] = *(s16 *)(*(char **)((char *)arg0 + 8) + 0x2E);
    matrix.t[2] = *(s16 *)(*(char **)((char *)arg0 + 8) + 0x32);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = 0xC8;
    scale.z = fx->scale;
    scratch_scale = scale;
    func_80078CC4(&matrix, &scratch_scale);

    func_800C6D5C(D_80190194, 0, 0);
    if (*(s32 *)(state + 0x24) == 0) {
        func_800C6EC0(func_80077A64(0, 1, 0x340, 0x100) & 0xFFFF,
                      func_80077AA4(0, 0x1D7) & 0xFFFF);
    }
    if (*(s32 *)(state + 0x24) == 1) {
        func_800C6EC0(func_80077A64(0, 1, 0x340, 0x160) & 0xFFFF,
                      func_80077AA4(0, 0x1DB) & 0xFFFF);
    }
    func_800C6ED8(1);

    func_800C6EF8(D_80190194);
    func_800C7098(D_80190194, 0x60, 0x10, 0x80);
    func_800C6FA0(D_80190194, fx->depth);
    func_800C71E4(D_80190194, &matrix);
    func_800C6F4C(D_80190194);
}
