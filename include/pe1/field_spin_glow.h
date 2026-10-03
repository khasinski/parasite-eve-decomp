#ifndef PE1_FIELD_SPIN_GLOW_H
#define PE1_FIELD_SPIN_GLOW_H

#include "common.h"

/* Falling glow task state: position, damped velocity with gravity, spin
 * angle and starting intensity. */
typedef struct FieldSpinGlow {
    s16 x, y, z;
    s16 vx, vy, vz;
    s16 angle;
    s16 intensity;
} FieldSpinGlow;

PE1_STATIC_ASSERT(sizeof(FieldSpinGlow) == 0x10, field_spin_glow_size);

int func_800DE7A8(int mode, FieldSpinGlow *state);

#endif
