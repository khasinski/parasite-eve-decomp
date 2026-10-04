#ifndef PE1_FIELD_FLASH_BURST_H
#define PE1_FIELD_FLASH_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_tile.h"

/* Field engine flash burst (func_800DE0A8): two screen flashes, a model
 * swelling on the actor's point 19 and a star fan, ring band and shape
 * quad at the anchor. */
typedef struct FieldFlashBurst {
    /* 0x00 */ GteShortVector anchor;
    /* 0x08 */ s16 point[3];
} FieldFlashBurst;

extern FieldActor *D_8009D254;
/* Model drawn by the burst and its colour track. */
extern u8 *D_800F3474;
extern u8 D_800E20CC[];

int rsin(int angle);
int rcos(int angle);
u16 GetClut(int x, int y);
GteMatrix *Gte_ScaleMatrix(GteMatrix *matrix, const GteVector *scale);

#endif /* PE1_FIELD_FLASH_BURST_H */
