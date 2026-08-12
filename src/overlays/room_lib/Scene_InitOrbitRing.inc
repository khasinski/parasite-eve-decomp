#include "common.h"
#include "pe1/overlay_math.h"

/* CC1_FLAGS: -fno-strength-reduce */

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
    unsigned char pad104;
    u8 phase;
    s16 speed;
    s16 verticalSpeed;
} SceneOrbitRing;

typedef struct SceneOrbitAnchor {
    unsigned char pad00[0x18];
    s32 x;
    s32 y;
    s32 z;
} SceneOrbitAnchor;

void SCENE_INIT_ORBIT_RING_NAME(int unused0, int unused1, SceneOrbitRing *ring) {
    SceneOrbitAnchor *anchor;
    unsigned int i;
    int angle;

    anchor = Overlay_GetScriptActor();
    i = 0;
    do {
        angle = i << 9;
        ring->point[i].x = anchor->x;
        ring->point[i].y = anchor->y - 0x64;
        ring->point[i].z = anchor->z;
        ring->direction[i].x = Overlay_Sin(angle);
        ring->direction[i].pad02 = 0;
        ring->direction[i].z = Overlay_Cos(angle);
        i++;
    } while (i < 16);

    ring->height = 0x80;
    ring->radius = 0x400;
    ring->verticalSpeed = -0x3C;
    ring->phase = 0;
    ring->speed = 0x320;
}
