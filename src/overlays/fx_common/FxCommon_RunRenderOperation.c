#include "fx_common.h"

int func_80191E30(int arg0)
{
    int size = 0x300;
    void *resource;
    void **matrixSlot;
    void *savedMatrix;
    void *savedOutput;
    int result;

    resource = func_8006EC6C(&D_801D0260, 3);
    func_80078A94();
    func_80078E94(&D_8019CC30);
    func_80078E04(&D_8019CC30);
    matrixSlot = &D_800BCFA4;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = D_800BCFA8;
    D_800BCFA8 = &size;
    result = func_8006DF50(resource, arg0, 0, 0x80, 1);
    *matrixSlot = savedMatrix;
    D_800BCFA8 = savedOutput;
    func_80078B38();
    return result;
}
