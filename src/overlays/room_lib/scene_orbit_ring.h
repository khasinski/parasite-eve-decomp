#ifndef PE1_SCENE_ORBIT_RING_H
#define PE1_SCENE_ORBIT_RING_H

#include "common.h"

typedef struct SceneOrbitPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad06;
} SceneOrbitPoint;

typedef struct SceneOrbitDirection {
    s16 x;
    s16 pad02;
    s16 z;
    s16 pad06;
} SceneOrbitDirection;

typedef struct SceneOrbitRing {
    SceneOrbitPoint point[16];
    SceneOrbitDirection direction[16];
    s16 radius;
    s16 height;
    u8 frame;
    u8 phase;
    s16 speed;
    s16 acceleration;
} SceneOrbitRing;

#endif
