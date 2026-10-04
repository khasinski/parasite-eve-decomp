#ifndef PE1_FIELD_STAR_FAN_H
#define PE1_FIELD_STAR_FAN_H

#include "common.h"
#include "pe1/field_rotated_triangle.h"

/* Field engine star fan (func_800D004C): a ring of gouraud triangles from
 * a common centre whose rim alternates between two radii, scaled, turned
 * and placed at a projected anchor. */

/* Unit scale used before the caller's X/Y scale is filled in. */
extern GteVector D_800C2290;

GteMatrix *Gte_ScaleMatrix(GteMatrix *matrix, const GteVector *scale);

#endif /* PE1_FIELD_STAR_FAN_H */
