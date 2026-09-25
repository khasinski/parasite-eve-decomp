#include "common.h"
#include "pe1/random.h"
#include "scene_particle_slots.h"

void Scene_InitParticleSlots_80191DB8(int unused0, int unused1, SceneParticleSlots *slots) {
    unsigned int i;

    slots->motionRamp = 0;
    slots->elapsedFrames = 0;
    for (i = 0; i < 4; i++) {
        slots->phase[i] = i << 11;
        slots->verticalOffset[i] = 0;
        slots->offset[i].x = 0;
        slots->offset[i].y = 0;
        slots->offset[i].z = 0;
        slots->timer[i] = Engine_Random() % 10 + 10;
    }
}
