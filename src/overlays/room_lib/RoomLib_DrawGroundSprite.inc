#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"

typedef struct RoomGroundSpriteStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomSpriteMatrix generatedMatrix;
    RoomFxVec4 scale;
} RoomGroundSpriteStack;

extern RoomFxSeed8 D_8018F02C;
extern RoomFxVec4 D_8018EFFC;
extern unsigned short ROOMLIB_DRAW_GROUND_SPRITE_DEPTH;
extern RoomSpriteMatrix *D_800BCFA4;
extern short ROOMLIB_DRAW_GROUND_SPRITE_FLOOR;

extern char *func_800C2B50(void);
extern void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
extern void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
extern void func_800C2EAC(unsigned char owner);
extern void func_800C2FF0(int width, int height);
extern void func_800C3098(int depth);
extern void func_800C3238(int mode);
extern void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);

void ROOMLIB_DRAW_GROUND_SPRITE_NAME(
    void *unused0, void *unused1, RoomFxGroundSpriteParams *fx) {
    RoomGroundSpriteStack stack;
    char *owner;
    unsigned short *depthSlot;
    RoomSpriteMatrix **matrixSlot;

    owner = func_800C2B50();
    stack.seed = D_8018F02C;
    func_800794C4(&stack.seed, &stack.generatedMatrix);
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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

    stack.scale = D_8018EFFC;
    func_80078CC4(&stack.matrix, &stack.scale);
    stack.matrix.t[0] = fx->x;
    stack.matrix.t[1] = ROOMLIB_DRAW_GROUND_SPRITE_FLOOR;
    stack.matrix.t[2] = fx->z;
    depthSlot = &ROOMLIB_DRAW_GROUND_SPRITE_DEPTH;
    *depthSlot = fx->depth;
    matrixSlot = &D_800BCFA4;
    gte_ldrotmatrix(*matrixSlot);
    gte_ldtransmatrix(*matrixSlot);
    func_800C42A4((char *)depthSlot - 0xA, &stack.matrix, 1);
}
