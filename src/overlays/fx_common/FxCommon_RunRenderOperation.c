#include "fx_common.h"

int func_80191E30(int arg0)
{
    int size = 0x300;
    void *resource;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    void *savedOutput;
    int result;

    resource = func_8006EC6C(&D_801D0260, 3);
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &D_800BCFA4.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = D_800BCFA8;
    D_800BCFA8 = &size;
    result = func_8006DF50(resource, arg0, 0, 0x80, 1);
    *matrixSlot = savedMatrix;
    D_800BCFA8 = savedOutput;
    PopMatrix();
    return result;
}
