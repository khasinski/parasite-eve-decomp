#ifndef PE1_ENTITY_FLOOR_H
#define PE1_ENTITY_FLOOR_H

/* Declarations used by the per-actor floor tracking driver
 * Entity_UpdateAndRender. */

#include "pe1/battle.h"
#include "pe1/field_collision.h"

/* The player slot and the field flags word lead larger records, so retail
 * addresses them absolutely (lui/lo) while the small collision globals
 * stay gp-relative. */
typedef struct FieldPlayerSlot {
    BattleEntity *actor;
    u32 reserved[3];
} FieldPlayerSlot;
typedef struct FieldFlagsBlock {
    u32 flags; /* 8: the player stands on a ramp edge */
    u32 reserved[3];
} FieldFlagsBlock;
extern FieldPlayerSlot D_8009D254;
extern FieldFlagsBlock D_8009D2E8;

int Math_FixedMul(int a, int b);
int Geo_ClipToFloorBoundary(s16 x, s16 z, void *face);

#endif /* PE1_ENTITY_FLOOR_H */
