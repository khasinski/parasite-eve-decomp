#ifndef PE1_GTE_TYPES_H
#define PE1_GTE_TYPES_H

#include "common.h"

typedef struct GteMatrix {
    s16 m[3][3];
    s32 t[3];
} GteMatrix;

typedef struct GteRotation {
    s16 x;
    s16 y;
    s16 z;
    s16 flags;
} GteRotation;

#endif
