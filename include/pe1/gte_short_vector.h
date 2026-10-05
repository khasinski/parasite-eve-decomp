#ifndef PE1_GTE_SHORT_VECTOR_H
#define PE1_GTE_SHORT_VECTOR_H

#include "common.h"

typedef struct GteShortVector {
    s16 x, y, z, pad;
} GteShortVector;

PE1_STATIC_ASSERT(sizeof(GteShortVector) == 8, gte_short_vector_size);

#endif /* PE1_GTE_SHORT_VECTOR_H */
