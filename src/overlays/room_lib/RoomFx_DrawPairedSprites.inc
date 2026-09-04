#include "common.h"
#include "pe1/room_fx.h"

extern RoomFxVec4 ROOMFX_DRAW_PAIRED_SPRITES_SCALE;
extern unsigned char ROOMFX_DRAW_PAIRED_SPRITES_PACKET[];
extern unsigned short ROOMFX_DRAW_PAIRED_SPRITES_DEPTH;
extern short g_FrameCount16;

extern char *func_800C2B50(void);
extern void func_80071A44(void *arg0, int arg1, int arg2);
extern void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
extern void func_800C2EAC(unsigned char owner);
extern void func_800C2FF0(int width, int height);
extern void func_800C3098(int depth);
extern void func_800C3238(int mode);
extern void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);

void ROOMFX_DRAW_PAIRED_SPRITES_NAME(
    void *unused0, void *unused1, RoomSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    unsigned short *depthSlot;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x10, 0x10);
    func_800C3098(0x10);
    func_800C3238(0);

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

    func_80071A44(&scale, 0, 0x10);
    scale.x = 0x100;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratchScale = scale;
    func_80078CC4(&matrix, &scratchScale);

    depthSlot = &ROOMFX_DRAW_PAIRED_SPRITES_DEPTH;
    *depthSlot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y - ((fx->scale - 0x100) >> 4);
    matrix.t[2] = fx->z;
    func_800C42A4((char *)depthSlot - 0xA, &matrix, 1);

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

    scale = ROOMFX_DRAW_PAIRED_SPRITES_SCALE;
    func_80078CC4(&matrix, &scale);
    func_800C3238(2);
    matrix.t[0] = fx->x;
    matrix.t[1] = g_FrameCount16;
    matrix.t[2] = fx->z;
    func_800C42A4(ROOMFX_DRAW_PAIRED_SPRITES_PACKET, &matrix, 1);
}
