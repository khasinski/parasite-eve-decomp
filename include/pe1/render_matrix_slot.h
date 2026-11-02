#ifndef PE1_RENDER_MATRIX_SLOT_H
#define PE1_RENDER_MATRIX_SLOT_H

#include "pe1/gte_types.h"

/* g_GeomScroll.words.viewMatrix, the current view matrix, as a record of
 * its own: effects load the matrix into the GTE rotation and translation
 * registers before projecting, and retail addresses the word itself
 * (la/lw 0) where the scroll-state view would load it at +0x1C from the
 * state's base. */
typedef struct RenderMatrixSlot {
    GteMatrix *value;
    u8 reserved[8];
} RenderMatrixSlot;

extern RenderMatrixSlot g_ViewMatrixSlot;
/* The old address spelling, still used by fieldfx/geometry.c. */
extern RenderMatrixSlot D_800BCFA4;
extern RenderMatrixSlot D_800BCA64;

#endif /* PE1_RENDER_MATRIX_SLOT_H */
