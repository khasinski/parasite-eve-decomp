#include "common.h"
#include "pe1/random.h"
#include "pe1/overlay_math.h"
#include "scene_e08.h"
#include "scene_particle_slots.h"

void func_80191B14(int unused, SceneMovingState *state, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; ++i) {
        if (slots->active[i] == 0 && Engine_Random() % 3 == 0) {
            slots->angle[i] = Engine_Random() % 4096;
            slots->initialSize[i] = 0x80;
            slots->active[i] = 1;
            slots->timer[i] = 90;
            slots->life[i] = 0;
            {
                int randomValue = Engine_Random();
                slots->radius[i] = 0;
                slots->range[i] = randomValue % 200 + 100;
            }
            {
                int randomValue = Engine_Random();
                int x = D_8019956C;
                int z = D_8019957C;
                slots->radius[i] = randomValue % 1500 + 300;
                slots->vector[i].y = 0;
                slots->vector[i].x = x;
                slots->vector[i].z = z;
            }
        }

        if (slots->active[i] == 1) {
            slots->life[i]++;
            if (--slots->timer[i] == 0) {
                slots->active[i] = 0;
            }
            slots->vector[i].dx = (Overlay_Sin(slots->angle[i]) * slots->radius[i] >> 12) + D_8019956C;
            slots->vector[i].dy = Overlay_Cos(slots->angle[i]) * slots->radius[i] >> 12;
            slots->vector[i].dz = (Overlay_Cos(slots->angle[i]) * slots->radius[i] >> 12) + D_8019957C;
            slots->vector[i].combinedX = (u16)slots->vector[i].x + (u16)slots->vector[i].dx;
            slots->vector[i].combinedY = (u16)slots->vector[i].y + (u16)slots->vector[i].dy;
            slots->vector[i].combinedZ = (u16)slots->vector[i].z + (u16)slots->vector[i].dz;
        }
    }
    if (state->elapsedFrames >= 181) {
        state->status = 2;
    }
}
