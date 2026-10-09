#ifndef PE1_FIXED_POINT_H
#define PE1_FIXED_POINT_H

#include "common.h"

/* Signed 16.16 world coordinates, with the little-endian halfword view
 * used by the geometry and actor-rendering code. */
typedef union Pe1Fixed16_16 {
    s32 fixed;
    struct { u16 frac; s16 integer; } parts;
} Pe1Fixed16_16;

/* FieldActor retains its scalar coordinate interface. Read the same
 * signed high halfword without converting the full fixed-point word. */
static inline s16 Pe1Fixed_Integer(const s32 *value)
{
    return ((const Pe1Fixed16_16 *)value)->parts.integer;
}

PE1_STATIC_ASSERT(sizeof(Pe1Fixed16_16) == 4, fixed_16_16_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1Fixed16_16, parts.integer) == 2,
                  fixed_16_16_integer_offset);

#endif
