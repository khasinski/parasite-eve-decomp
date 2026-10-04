#ifndef PE1_FLOOR_CLIP_H
#define PE1_FLOOR_CLIP_H

/* Declarations used by the floor boundary walk Geo_ClipToFloorBoundary. */

#include "pe1/field_collision.h"

/* Floor edge record (D_8009CE14, one per edge id): the 16.16 length,
 * whose integer half is RampEdge.length, and the unit direction. */
typedef union FloorEdge {
    RampEdge ramp;
    s32 length;
} FloorEdge;

/* Neighbour list initialiser: three -1 entries. */
typedef struct FloorNeighbours {
    s16 index[3];
} FloorNeighbours;

extern FloorNeighbours D_8009CD88;
/* One bit per edge id, set when the walk has tested that edge. */
extern u32 D_8009DFB0[];
extern s16 D_8009CE1C, D_8009CE20, D_8009CE24, D_8009CE28;

int Math_FixedMul(int lhs, int rhs);
int Geo_ClipToFloorBoundary(s16 x, s16 z, u16 *triangle);

#endif /* PE1_FLOOR_CLIP_H */
