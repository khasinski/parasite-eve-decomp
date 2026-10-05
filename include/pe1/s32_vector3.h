#ifndef PE1_S32_VECTOR3_H
#define PE1_S32_VECTOR3_H

#include "common.h"

typedef struct Pe1S32Vector3 {
    s32 x;
    s32 y;
    s32 z;
} Pe1S32Vector3;

PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1S32Vector3, y) == 4,
                  pe1_s32_vector3_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1S32Vector3, z) == 8,
                  pe1_s32_vector3_z_offset);
PE1_STATIC_ASSERT(sizeof(Pe1S32Vector3) == 12,
                  pe1_s32_vector3_size);

#endif /* PE1_S32_VECTOR3_H */
