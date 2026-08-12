#include "common.h"
#include "pe1/overlay_math.h"
#include "scene_orbit_ring.h"

/* CC1_FLAGS: -fno-strength-reduce */

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
    ring->acceleration = -0x3C;
    ring->phase = 0;
    ring->speed = 0x320;
}
