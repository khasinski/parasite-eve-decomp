#include "common.h"
#include "pe1/random.h"
#include "pe1/overlay_math.h"
#include "scene_e08.h"
#include "scene_particle_slots.h"

void func_80192904(int unused, SceneMovingState *state, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; ++i) {
        if (slots->active[i] == 0) {
            if (Engine_Random() % 15 == 0 && state->elapsedFrames < 120) {
                slots->angle[i] = Engine_Random() % 4096;
                slots->initialSize[i] = 0x80;
                slots->active[i] = 1;
                {
                    int randomValue = Engine_Random();
                    slots->radius[i] = 2000;
                    slots->extraRadius[i] = 2400;
                    slots->range[i] = randomValue % 600 + 100;
                }
                slots->life[i] = 0;
            }
        }
        if (slots->active[i] != 0) {
            slots->angle[i] = (u16)slots->angle[i] + ((s16)slots->range[i] >> 4);
            if (slots->active[i] == 1) {
                if (slots->extraRadius[i] < 20) {
                    slots->active[i] = 2;
                }
                slots->extraRadius[i] = (u16)slots->extraRadius[i] - 100;
                slots->radius[i] = (u16)slots->radius[i] - 20;
            }
            if (slots->active[i] == 2) {
                if (slots->radius[i] < 0) {
                    slots->active[i] = 0;
                }
                slots->extraRadius[i] = 0;
                slots->radius[i] = (u16)slots->radius[i] - 120;
            }
            slots->life[i]++;
            slots->vector[i].dx = (Overlay_Sin(slots->angle[i]) *
                                   (slots->extraRadius[i] + slots->radius[i]) >> 12) + D_8019956C;
            slots->vector[i].dy = Overlay_Cos(slots->angle[i]) *
                                  (slots->extraRadius[i] + slots->radius[i]) >> 12;
            slots->vector[i].dz = (Overlay_Cos(slots->angle[i]) *
                                   (slots->extraRadius[i] + slots->radius[i]) >> 12) + D_8019957C;
            slots->vector[i].x = (Overlay_Sin(slots->angle[i]) * slots->extraRadius[i] >> 12) + D_8019956C;
            slots->vector[i].y = Overlay_Cos(slots->angle[i]) * slots->extraRadius[i] >> 12;
            slots->vector[i].z = (Overlay_Cos(slots->angle[i]) * slots->extraRadius[i] >> 12) + D_8019957C;
            slots->vector[i].combinedX = ((s16)slots->vector[i].x + (s16)slots->vector[i].dx) >> 1;
            slots->vector[i].combinedY = ((s16)slots->vector[i].y + (s16)slots->vector[i].dy) >> 1;
            slots->vector[i].combinedZ = ((s16)slots->vector[i].z + (s16)slots->vector[i].dz) >> 1;
        }
    }
    if (state->elapsedFrames >= 181) {
        state->status = 2;
    }
}
