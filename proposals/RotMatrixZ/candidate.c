/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"

typedef struct RotationMatrix {
    s16 m[3][3];
    s16 padding;
    s32 translation[3];
} RotationMatrix;
typedef char RotationMatrixSizeCheck[(sizeof(RotationMatrix) == 32) ? 1 : -1];
extern u32 D_800966EC[4096];

RotationMatrix *RotMatrixZ(s32 angle, RotationMatrix *matrix) {
    u32 packed;
    s32 sine, cosine;
    s32 x0, x1, x2, y0, y1, y2;
    if (angle < 0) {
        packed = D_800966EC[(0U - (u32)angle) & 0xFFF];
        sine = -(s32)(s16)packed;
    } else {
        packed = D_800966EC[(u32)angle & 0xFFF];
        sine = (s16)packed;
    }
    cosine = (s32)packed >> 16;
    x0 = matrix->m[0][0];
    y0 = matrix->m[1][0];
    x1 = matrix->m[0][1];
    y1 = matrix->m[1][1];
    x2 = matrix->m[0][2];
    y2 = matrix->m[1][2];
    matrix->m[0][0] = (s32)((u32)cosine * x0 - (u32)sine * y0) >> 12;
    matrix->m[0][1] = (s32)((u32)cosine * x1 - (u32)sine * y1) >> 12;
    matrix->m[0][2] = (s32)((u32)cosine * x2 - (u32)sine * y2) >> 12;
    matrix->m[1][0] = (s32)((u32)sine * x0 + (u32)cosine * y0) >> 12;
    matrix->m[1][1] = (s32)((u32)sine * x1 + (u32)cosine * y1) >> 12;
    matrix->m[1][2] = (s32)((u32)sine * x2 + (u32)cosine * y2) >> 12;
    return matrix;
}
