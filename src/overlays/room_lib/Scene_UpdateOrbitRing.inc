#include "scene_orbit_ring.h"

typedef struct SceneOrbitScript {
    u8 state;
    u8 result;
} SceneOrbitScript;

void SCENE_UPDATE_ORBIT_RING_NAME(
    int unused, SceneOrbitScript *script, SceneOrbitRing *ring) {
    unsigned int i;
    u8 phase;
    u8 frame;
    int product;

    ring->speed = ring->speed + ring->acceleration;
    if (ring->height >= 0x11) {
        ring->height -= 0x10;
    }

    for (i = 0; i < 16; i++) {
        product = ring->direction[i].x * ring->speed;
        ring->point[i].y += 8;
        ring->point[i].x += product >> 16;
        product = ring->direction[i].z * ring->speed;
        ring->point[i].z += product >> 16;
    }

    phase = ring->phase + 6;
    ring->phase = phase;
    frame = phase >> 4;
    ring->frame = frame + 1;
    if (frame == 7) {
        ring->frame = 0;
        script->result = 2;
    }
}
