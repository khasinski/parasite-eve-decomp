#include "common.h"
#include "pe1/room_fx.h"

void func_801904DC(void *unused, u8 *control, RoomOrbitParticleState *state) {
    unsigned int i;
    u8 intensity;
    u16 height;

    state->radius += state->radiusStep;
    if (state->decay >= 9) {
        state->decay -= 8;
    }

    i = 0;
    while (i < 8) {
        s32 xDelta =
            ((s32)state->velocity[i].x * state->radius) >> 16;

        state->position[i].y += (s8)(state->velocity[i].angle >> 8);
        state->position[i].x += xDelta;
        state->position[i].z +=
            ((s32)state->velocity[i].z * state->radius) >> 16;
        state->velocity[i].angle += 0x258;
        i++;
    }

    height = state->height + 0x46;
    intensity = state->intensity + 6;
    state->intensity = intensity;
    intensity >>= 4;
    state->height = height;
    state->frame = intensity + 1;
    if (intensity == 7) {
        state->frame = 0;
        control[1] = 2;
    }
}
