#ifndef PE1_VECTOR_TYPES_H
#define PE1_VECTOR_TYPES_H

#include "common.h"

typedef struct Pe1Vec3s {
    s16 x;
    s16 y;
    s16 z;
} Pe1Vec3s;

PE1_STATIC_ASSERT(sizeof(Pe1Vec3s) == 6, pe1_vec3s_size);

#endif
