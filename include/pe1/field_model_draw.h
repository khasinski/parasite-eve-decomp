#ifndef PE1_FIELD_MODEL_DRAW_H
#define PE1_FIELD_MODEL_DRAW_H

#include "common.h"
#include "pe1/gte_types.h"

/* Model packet helpers used by the main executable's field engine flares.
 * Room overlays declare the same helpers with their own parameter types, so
 * these prototypes live apart from field_anim.h. */
void GsSetOrign(int tpage, int clut);
void func_800C6ED8(int mode);
void func_800C6EF8(u8 *data);
void func_800C6F4C(u8 *data);
void func_800C7098(u8 *data, int r, int g, int b);
void func_800C71E4(u8 *data, GteMatrix *matrix);

#endif
