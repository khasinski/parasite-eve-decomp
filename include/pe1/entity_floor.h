#ifndef PE1_ENTITY_FLOOR_H
#define PE1_ENTITY_FLOOR_H

/* Declarations used by the per-actor floor tracking driver
 * Entity_UpdateAndRender. */

#include "pe1/battle.h"
#include "pe1/field_collision.h"

/* The player slot and the field flags word lead 8-byte records: small for
 * cc1 at -G8 (direct lw/sw), absolute for the assembler at -G4, as retail. */
typedef struct FieldPlayerSlot {
    BattleEntity *actor;
    u32 reserved;
} FieldPlayerSlot;
typedef struct FieldFlagsBlock {
    u32 flags; /* 8: the player stands on a ramp edge */
    u32 reserved;
} FieldFlagsBlock;
extern FieldPlayerSlot D_8009D254;
extern FieldFlagsBlock D_8009D2E8;

int abs(int value);
int Math_FixedMul(int a, int b);
int Geo_ClipToFloorBoundary(s16 x, s16 z, void *face);

#endif /* PE1_ENTITY_FLOOR_H */
