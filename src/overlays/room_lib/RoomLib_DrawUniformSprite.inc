#include "pe1/field_script_context.h"
extern RoomFxSeed8 ROOMLIB_DRAW_UNIFORM_SPRITE_SEED;
extern u16 ROOMLIB_DRAW_UNIFORM_SPRITE_DEPTH;
extern void func_80071A44(void *arg0, int arg1, int arg2);
#include "pe1/gte_types.h"
extern void func_800C2EAC(u8 arg0);
extern void func_800C2FF0(int arg0, int arg1);
extern void func_800C3098(int arg0);
extern void func_800C3238(int arg0);
extern void func_800C42A4(void *arg0, RoomSpriteMatrix *matrix, int arg2);

void ROOMLIB_DRAW_UNIFORM_SPRITE_NAME(
    void *unused0, void *unused1, RoomUniformSpriteFxParams *fx) {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomSpriteMatrix *matrixPtr;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    u16 *depthSlot;

    owner = func_800C2B50();
    matrixPtr = &matrix;
    seed = ROOMLIB_DRAW_UNIFORM_SPRITE_SEED;
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(1);
    RotMatrix(&seed, matrixPtr);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = fx->scale;
    scratchScale = scale;
    ScaleMatrix(matrixPtr, &scratchScale);

    depthSlot = &ROOMLIB_DRAW_UNIFORM_SPRITE_DEPTH;
    *depthSlot = fx->depth;
    matrixPtr->t[0] = fx->x;
    matrixPtr->t[1] = fx->y;
    matrixPtr->t[2] = fx->z;
    func_800C42A4((char *)depthSlot - 0xA, matrixPtr, 0);
}
