#include "common.h"
#include "pe1/random.h"
#include "pe1/overlay_math.h"

#include "scene_particle_slots.h"

void func_801921C8(int unused, u8 *state, SceneParticleSlots *slots)
{
    unsigned int i;
    u16 oldPhase;
    ++slots->elapsedFrames;
    if (slots->motionRamp < 0x1000) {
        slots->motionRamp += 200;
    }
    for (i = 0; i < 4; ++i) {
        slots->cycle[i] = (s16)((u16)slots->cycle[i] + 1) % 40;
        slots->phase[i] -= 200;
        slots->offset[i].x = Overlay_Sin(((s16 *)state)[1] << 3) >> 3;
        slots->offset[i].y = Overlay_Cos(((s16 *)state)[1] << 4) >> 3;
        oldPhase = (u16)slots->offset[i].z;
        slots->offset[i].x = 0;
        slots->offset[i].y = 0;
        slots->offset[i].z = oldPhase + (u16)slots->timer[i];
        if (slots->verticalOffset[i] < 0x80) {
            slots->verticalOffset[i] += 8;
        }
        if (slots->phase[i] < 0) {
            slots->cycle[i] = Engine_Random() % 10;
            slots->phase[i] = 0x2000;
            slots->verticalOffset[i] = 0;
            slots->offset[i].z = Engine_Random() % 4096;
            slots->timer[i] = Engine_Random() % 10 + 20;
        }
    }
    if (slots->elapsedFrames >= 181) {
        state[1] = 2;
    }
}
