#ifndef SCENE_E08_PARTICLE_SLOTS_H
#define SCENE_E08_PARTICLE_SLOTS_H

#include "common.h"

typedef struct SceneParticleOffset {
    s16 x;
    s16 y;
    s16 z;
    s16 pad06;
} SceneParticleOffset;

typedef struct SceneParticleSlots {
    u8 pad00[8];
    SceneParticleOffset offset[4];
    s32 phase[4];
    s16 verticalOffset[4];
    u8 pad40[8];
    s16 timer[4];
    s16 cycle[4];
    s16 motionRamp;
    s16 elapsedFrames;
} SceneParticleSlots;

typedef struct SceneEmitterSlots {
    SceneParticleOffset position[4];
    s16 field20[4];
    s16 field28[4];
    u8 field30[4];
    u8 field34[4];
    u8 field38[4];
    u8 field3C[4];
    u8 field40[4];
    u8 activeSlots;
} SceneEmitterSlots;

#endif
