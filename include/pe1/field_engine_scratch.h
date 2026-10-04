#ifndef PE1_FIELD_ENGINE_SCRATCH_H
#define PE1_FIELD_ENGINE_SCRATCH_H

#include "common.h"
#include "pe1/gte_types.h"

/* Scratchpad work area the field engine draw routines publish through
 * D_800F33B4: an ordering-table depth, the current texture cell, the
 * CLUT offset and a composed matrix. */
typedef struct FieldEngineScratch {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s32 depth;
    /* 0x10 */ u8 u;
    /* 0x11 */ u8 v;
    /* 0x12 */ u8 clutX;
    /* 0x13 */ u8 clutY;
    /* 0x14 */ u8 pad14[8];
    /* 0x1C */ GteMatrix matrix;
} FieldEngineScratch;

#define FIELD_ENGINE_SCRATCH ((FieldEngineScratch *)0x1F800000)

extern FieldEngineScratch *D_800F33B4;

#endif
