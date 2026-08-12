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
    void **matrixSlot;
    void *savedMatrix;
    void *savedOutput;

    scratch.size = 0x300;
    func_80078A94();
    func_80078E94(&D_8019CC30);
    func_80078E04(&D_8019CC30);
    matrixSlot = &D_800BCFA4;
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
    func_80078B38();
}
