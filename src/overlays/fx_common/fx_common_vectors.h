#ifndef FX_COMMON_VECTORS_H
#define FX_COMMON_VECTORS_H

#include "common.h"

typedef struct FxCommonVec2 {
    s32 x;
    s32 y;
} FxCommonVec2;

typedef struct FxCommonVec3 {
    s32 x;
    s32 y;
    s32 z;
} FxCommonVec3;

PE1_STATIC_ASSERT(sizeof(FxCommonVec2) == 8, fx_common_vec2_size);
PE1_STATIC_ASSERT(sizeof(FxCommonVec3) == 12, fx_common_vec3_size);

#endif /* FX_COMMON_VECTORS_H */
