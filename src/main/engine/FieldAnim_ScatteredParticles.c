#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/field_anim.h"
#include "pe1/field_engine_state.h"

void func_800CC2C4(void *arg0, void *arg1, FieldAnimScatteredParticles *state)
{
    int i;

    state->points.scale = 127;
    if (((Combatant *)D_8009D254->core)->action->actionCode.actionId == 3)
        state->points.count = 4;
    else
        state->points.count = 16;

    for (i = 0; i < state->points.count; i++) {
        state->points.x[i] = D_800E2290.x;
        state->points.y[i] = D_800E2290.y;
        state->points.z[i] = D_800E2290.z;
        state->velocity_x[i] = (rand() % 11 - 5) * 256;
        state->velocity_y[i] = -(rand() % 16 + 20) * 256;
        state->velocity_z[i] = (rand() % 11 - 5) * 256;
    }
}
