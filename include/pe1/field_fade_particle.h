#ifndef PE1_FIELD_FADE_PARTICLE_H
#define PE1_FIELD_FADE_PARTICLE_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_effect_pool.h"

/* Field engine fade particle (func_800DEFFC), spawned by the fading
 * emitter func_800DF6AC: a glow pinned to one actor matrix point that
 * sheds drifting and falling sparks into the same pool. */

extern FieldActor *D_8009D254;

int rand(void);
int rsin(int angle);
int rcos(int angle);
u16 GetClut(int x, int y);

#endif /* PE1_FIELD_FADE_PARTICLE_H */
