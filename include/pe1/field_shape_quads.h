#ifndef PE1_FIELD_SHAPE_QUADS_H
#define PE1_FIELD_SHAPE_QUADS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/render_object.h"
#include "pe1/field_textured_strip.h"

/* Field engine shape quads (func_800CEE20): every quad of the selected
 * shape is projected with a YXZ rotation scaled by the texture cell size
 * and drawn as a copy of one POLY_FT4 template. */

/* Quads (four vertices each) and quad counts of the shapes. */
extern GteShortVector *D_800E13BC[];
extern u16 D_800E1210[];

/* Rotation used when the caller passes none. */
extern GteRotation D_800C2268;

void MulRotMatrix(GteMatrix *matrix);

#endif
