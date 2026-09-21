#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

void MulRotMatrix(GteMatrix *matrix);

void func_800CF658(GteRotation *rotation, s32 *scale, GteMatrix *matrix) {
    GteMatrix local;

    if (matrix == 0) {
        matrix = &local;
    }
    RotMatrixYXZ((GteShortVector *)rotation, matrix);
    if (scale != 0) {
        Gte_ScaleMatrix(matrix, (const GteVector *)scale);
    }
    if (rotation->flags != 0) {
        MulRotMatrix(matrix);
    }
    gte_ldrotmatrix(matrix);
}
