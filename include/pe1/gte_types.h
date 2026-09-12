#ifndef PE1_GTE_TYPES_H
#define PE1_GTE_TYPES_H

#include "common.h"

typedef struct GteMatrix {
    s16 m[3][3];
    s32 t[3];
} GteMatrix;

typedef struct GteShortVector {
    s16 x, y, z, pad;
} GteShortVector;

typedef struct GteVector {
    s32 x, y, z, pad;
} GteVector;

PE1_STATIC_ASSERT(sizeof(GteMatrix) == 32, gte_matrix_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GteMatrix, t) == 20, gte_matrix_translation_offset);
PE1_STATIC_ASSERT(sizeof(GteShortVector) == 8, gte_short_vector_size);
PE1_STATIC_ASSERT(sizeof(GteVector) == 16, gte_vector_size);

typedef struct GteRotation {
    s16 x;
    s16 y;
    s16 z;
    s16 flags;
} GteRotation;

#endif
