#ifndef PE1_RENDER_MATRIX_SLOT_H
#define PE1_RENDER_MATRIX_SLOT_H

#include "pe1/gte_types.h"

/* Current view matrix slot (0x800BCFA4): effects load its matrix into the
 * GTE rotation and translation registers before projecting. */
typedef struct RenderMatrixSlot {
    GteMatrix *value;
    u8 reserved[8];
} RenderMatrixSlot;

extern RenderMatrixSlot D_800BCFA4;

#endif /* PE1_RENDER_MATRIX_SLOT_H */
