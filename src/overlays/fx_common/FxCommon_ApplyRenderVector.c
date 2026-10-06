#include "fx_common.h"

void func_80191EFC(void *object, volatile FxCommonVec3 *input)
{
    struct FxCommonApplyScratch {
        int size;
        int pad14;
        FxCommonShortVec3 vector;
        s16 pad1E;
        int valueA;
        int valueB;
    } scratch;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    void *savedOutput;

    scratch.size = 0x300;
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &D_800BCFA4.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = D_800BCFA8;
    D_800BCFA8 = (void *)&scratch.size;
    scratch.vector.x = input->x;
    scratch.vector.y = input->y;
    scratch.vector.z = input->z;
    func_8006DFA8(&scratch.vector, &scratch.valueA, &scratch.valueB);
    func_800868F0(object, 0, scratch.valueB);
    func_80086A28(object, 0, scratch.valueA);
    *matrixSlot = savedMatrix;
    D_800BCFA8 = savedOutput;
    PopMatrix();
}
