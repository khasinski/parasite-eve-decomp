#ifndef PE1_FIELD_GLOW_FOUNTAIN_H
#define PE1_FIELD_GLOW_FOUNTAIN_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_spin_glow.h"

/* Field engine glow fountain (func_800DEA30): sprays falling spin glows
 * (func_800DE7A8) out of the actor's facing for 16 frames while a rising
 * shape quad, a ring and a spinning band are drawn over it. */
typedef struct FieldGlowFountain {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 reserved06;
    /* 0x08 */ int seed;
} FieldGlowFountain;

PE1_STATIC_ASSERT(sizeof(FieldGlowFountain) == 0x0C, field_glow_fountain_size);

extern FieldActor *D_8009D254;

int rand(void);
int rsin(int angle);
int rcos(int angle);
u16 GetClut(int x, int y);

#endif /* PE1_FIELD_GLOW_FOUNTAIN_H */
