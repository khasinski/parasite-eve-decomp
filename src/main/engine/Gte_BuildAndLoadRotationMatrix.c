#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

void RotMatrixYXZ(GteRotation *rotation, GteMatrix *matrix);
void Gte_ScaleMatrix(GteMatrix *matrix, s32 *scale);
void MulRotMatrix(GteMatrix *matrix);

void func_800CF658(GteRotation *rotation, s32 *scale, GteMatrix *matrix) {
    GteMatrix local;

    if (matrix == 0) {
        matrix = &local;
    }
    RotMatrixYXZ(rotation, matrix);
    if (scale != 0) {
        Gte_ScaleMatrix(matrix, scale);
    }
    if (rotation->flags != 0) {
        MulRotMatrix(matrix);
    }
    gte_ldrotmatrix(matrix);
}
