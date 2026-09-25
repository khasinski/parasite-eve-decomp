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

typedef struct SceneMovingVector {
    s16 x;
    s16 y;
    s16 z;
    s16 pad06;
    s16 combinedX;
    s16 combinedY;
    s16 combinedZ;
    s16 pad0E;
    s16 dx;
    s16 dy;
    s16 dz;
    s16 pad16;
} SceneMovingVector;

typedef struct SceneMovingSlots {
    SceneMovingVector vector[16];
    s32 life[16];
    s16 active[16];
    s16 timer[16];
    s16 initialSize[16];
    s16 angle[16];
    s16 range[16];
    s16 radius[16];
    s16 extraRadius[16];
} SceneMovingSlots;

typedef struct SceneMovingState {
    u8 pad00;
    u8 status;
    s16 elapsedFrames;
} SceneMovingState;

typedef struct SceneParticleCluster {
    SceneParticleOffset position[30];
    s16 fieldF0[30];
    s16 angle[30];
    s16 brightness[30];
    u8 texture[30];
    u8 mode[30];
    u8 counter[30];
    u8 field1FE[30];
    u8 active[30];
    u8 timer[30];
    u8 count;
} SceneParticleCluster;

#endif
