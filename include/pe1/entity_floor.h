#ifndef PE1_ENTITY_FLOOR_H
#define PE1_ENTITY_FLOOR_H

/* Declarations used by the per-actor floor tracking driver
 * Entity_UpdateAndRender. */

#include "pe1/battle.h"
#include "pe1/field_collision.h"

/* The floor driver reads the player and the field flags word (8: the player
 * stands on a ramp edge) absolutely; its unit runs under MASPSX_FORCE_G0. */
extern BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern unsigned int D_8009D2E8;
extern unsigned int D_8009CDA8;

int abs(int value);
int Geo_ClipToFloorBoundary(s16 x, s16 z, void *face);

#endif /* PE1_ENTITY_FLOOR_H */
