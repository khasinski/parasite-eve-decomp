#ifndef PE1_FLOOR_CLIP_H
#define PE1_FLOOR_CLIP_H

/* Declarations used by the floor boundary walk Geo_ClipToFloorBoundary. */

#include "pe1/field_collision.h"

/* Neighbour list initialiser: three -1 entries. */
typedef struct FloorNeighbours {
    s16 index[3];
} FloorNeighbours;

extern FloorNeighbours D_8009CD88;

int Math_FixedMul(int lhs, int rhs);
int Geo_ClipToFloorBoundary(s16 x, s16 z, void *triangle);

#endif /* PE1_FLOOR_CLIP_H */
