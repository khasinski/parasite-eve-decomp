#include "common.h"
#include "pe1/gte.h"

typedef struct RotationMatrix {
    s16 m[3][3];
    s16 padding;
    s32 translation[3];
} RotationMatrix;
typedef char RotationMatrixSizeCheck[(sizeof(RotationMatrix) == 32) ? 1 : -1];

#define rotate_column(x, y, z, out_x, out_y, out_z) do { \
    u32 xy = (u16)(x) | ((u32)(u16)(y) << 16); \
    gte_mtc2_0(xy); \
    asm volatile("mtc2 %0,$1" : : "r"(z)); \
    gte_mvmva(); \
    gte_getir1(out_x); \
    gte_getir2(out_y); \
    gte_getir3(out_z); \
} while (0)

RotationMatrix *MulRotMatrix(RotationMatrix *matrix) {
    s32 a, b, c, d, e, f, g, h, i;
    rotate_column(matrix->m[0][0], matrix->m[1][0], matrix->m[2][0], a, b, c);
    rotate_column(matrix->m[0][1], matrix->m[1][1], matrix->m[2][1], d, e, f);
    rotate_column(matrix->m[0][2], matrix->m[1][2], matrix->m[2][2], g, h, i);
    matrix->m[0][0] = a;
    matrix->m[0][1] = d;
    matrix->m[0][2] = g;
    matrix->m[1][0] = b;
    matrix->m[1][1] = e;
    matrix->m[1][2] = h;
    matrix->m[2][0] = c;
    matrix->m[2][1] = f;
    matrix->m[2][2] = i;
    matrix->padding = i >> 16;
    return matrix;
}
