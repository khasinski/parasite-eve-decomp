#ifndef FX_COMMON_VECTORS_H
#define FX_COMMON_VECTORS_H

#include "common.h"
#include "pe1/s32_vector3.h"

typedef struct FxCommonVec2 {
    s32 x;
    s32 y;
} FxCommonVec2;

typedef Pe1S32Vector3 FxCommonVec3;

PE1_STATIC_ASSERT(sizeof(FxCommonVec2) == 8, fx_common_vec2_size);
PE1_STATIC_ASSERT(sizeof(FxCommonVec3) == 12, fx_common_vec3_size);

#endif /* FX_COMMON_VECTORS_H */
