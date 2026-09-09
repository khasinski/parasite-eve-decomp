#include "common.h"

typedef struct RotationMatrix {
    s16 m[3][3];
    s16 padding;
    s32 translation[3];
} RotationMatrix;
typedef char RotationMatrixSizeCheck[(sizeof(RotationMatrix) == 32) ? 1 : -1];
extern u32 D_800966EC[4096];

static __inline__ void rotation_trig(s32 angle, s32 *sine, s32 *cosine) {
    u32 packed = D_800966EC[(angle < 0 ? 0U - (u32)angle : (u32)angle) & 0xFFF];
    *sine = angle < 0 ? -(s32)(s16)packed : (s32)(s16)packed;
    *cosine = (s32)packed >> 16;
}

static __inline__ s32 q12(s32 a, s32 b) {
    return (s32)((u32)a * b) >> 12;
}

RotationMatrix *RotMatrixYXZ(const s16 *angles, RotationMatrix *matrix) {
    s32 sx, cx, sy, cy, sz, cz;
    s32 a, b;
    s32 x = angles[0], y = angles[1], z = angles[2];
    rotation_trig(x, &sx, &cx);
    rotation_trig(y, &sy, &cy);
    rotation_trig(z, &sz, &cz);
    matrix->m[1][2] = -sx;
    matrix->m[0][2] = q12(sy, cx);
    matrix->m[2][2] = q12(cy, cx);
    matrix->m[1][0] = q12(sz, cx);
    matrix->m[1][1] = q12(cz, cx);
    a = q12(sy, sx);
    matrix->m[0][0] = q12(cy, cz) + q12(a, sz);
    matrix->m[0][1] = -q12(cy, sz) + q12(a, cz);
    b = q12(cy, sx);
    matrix->m[2][1] = q12(sy, sz) + q12(b, cz);
    matrix->m[2][0] = -q12(sy, cz) + q12(b, sz);
    return matrix;
}
