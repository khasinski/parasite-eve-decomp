#include "common.h"

typedef struct ScaleMatrix {
    s16 m[3][3];
    s16 padding;
    s32 translation[3];
} ScaleMatrix;
typedef char ScaleMatrixSizeCheck[(sizeof(ScaleMatrix) == 32) ? 1 : -1];

static __inline__ s32 scale_q12(s32 value, s32 scale) {
    return (s32)((u32)value * (u32)scale) >> 12;
}

ScaleMatrix *Gte_ScaleMatrix(ScaleMatrix *matrix, const s32 *scale) {
    s32 x = scale[0], y = scale[1], z = scale[2];
    s32 last;
    matrix->m[0][0] = scale_q12(matrix->m[0][0], x);
    matrix->m[0][1] = scale_q12(matrix->m[0][1], y);
    matrix->m[0][2] = scale_q12(matrix->m[0][2], z);
    matrix->m[1][0] = scale_q12(matrix->m[1][0], x);
    matrix->m[1][1] = scale_q12(matrix->m[1][1], y);
    matrix->m[1][2] = scale_q12(matrix->m[1][2], z);
    matrix->m[2][0] = scale_q12(matrix->m[2][0], x);
    matrix->m[2][1] = scale_q12(matrix->m[2][1], y);
    last = scale_q12(matrix->m[2][2], z);
    matrix->m[2][2] = last;
    matrix->padding = last >> 16;
    return matrix;
}
