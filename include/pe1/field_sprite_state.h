#ifndef PE1_FIELD_SPRITE_STATE_H
#define PE1_FIELD_SPRITE_STATE_H

#include "common.h"

/* Field engine sprite draw state, owned by engine/FieldEng_SpriteRendering.c:
 * the texture page and CLUT origin the sprite draws use, the semi-transparency
 * mode and the cell size. The corners of the sprite quad that
 * func_800C2FF0 sizes are declared with the shaded quad (field_shaded_quad.h).
 * Effects set it up before drawing through func_800C2EAC (page origin and
 * CLUT row), func_800C3098 (colour depth), func_800C3238 (blend mode) and
 * func_800C2FF0 (cell size in texels). */

extern u8 D_800F33AC;  /* texture colour mode: 0 = 4-bit, 1 = 8-bit */
extern u8 D_800E224C;  /* semi-transparency rate (GetTPage abr) */
extern u8 D_800F33B8;  /* blend mode last set by func_800C3238 */
extern u8 D_800F337A;  /* draw the sprites semi-transparent */
extern u16 D_800F3424; /* texture page origin x */
extern u16 D_800F3426; /* texture page origin y */
extern u16 D_800E27AC; /* GetTPage value of the current state */
extern u16 D_800F341C; /* CLUT origin x */
extern u16 D_800F341E; /* CLUT origin y */
extern u8 D_800F3422;
extern u8 D_800F345C;  /* cell width - 1 */
extern u8 D_800F345D;  /* cell height - 1 */

void func_800C2EAC(u8 mode);
void func_800C2FF0(int width, int height);
void func_800C3098(int mode);
void func_800C3134(u8 *table, u32 step, u8 *out);
void func_800C3238(u8 mode);

#endif
