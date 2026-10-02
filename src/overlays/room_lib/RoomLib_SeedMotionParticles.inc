#include "pe1/field_script_context.h"
#include "../room_lib/RoomLib_DrawMotion.h"
#ifndef ROOMLIB_SEED_MOTION_PARTICLES_FUNC
#error "ROOMLIB_SEED_MOTION_PARTICLES_FUNC must name the room entry point"
#endif

s32 *func_800C2B10(s32 index);
int func_80071A54(void);

/* Seeds the motion particles: three or six of them depending on the first
 * scene flag, each with a random anchor offset, a random sprite lookup and
 * a random drift that is slower and sideways when the flag is set. */
void ROOMLIB_SEED_MOTION_PARTICLES_FUNC(void *unused0, void *unused1,
                                        RoomMotionState *state)
{
    /* Retail reserves one 8-byte local that nothing in the shipped code
     * reads; keep it so the frame matches. */
    s16 unusedVector[4];
    unsigned int i;

    if (*func_800C2B10(1)) {
        state->count = 3;
    } else {
        state->count = 6;
    }
    state->depth = 0;
    i = 0;
    if (state->count != 0) {
        do {
            state->scale[i].x = 0x400;
            state->scale[i].y = 0x400;
            state->particle[i].primaryX = func_80071A54() % 140 - 70;
            state->particle[i].primaryY = func_80071A54() % 140 - 70;
            state->particle[i].primaryZ = func_80071A54() % 140 - 70;
            state->particle[i].secondaryX = func_80071A54() % 140 - 70;
            state->particle[i].secondaryY = func_80071A54() % 140 - 70;
            state->particle[i].secondaryZ = func_80071A54() % 140 - 70;
            state->lookup[i].id = func_80071A54() % 8;
            state->lookup[i].pad = func_80071A54() % 8;
            if (*func_800C2B10(1) == 0) {
                state->velocity[i].primaryX = func_80071A54() % 40 - 20;
                state->velocity[i].primaryY = 0;
                state->velocity[i].primaryZ = func_80071A54() % 40 - 20;
                state->velocity[i].secondaryX = func_80071A54() % 40 - 20;
                state->velocity[i].secondaryY = 0;
                state->velocity[i].secondaryZ = func_80071A54() % 40 - 20;
            } else {
                state->velocity[i].primaryX = func_80071A54() % 26 - 13;
                state->velocity[i].primaryY = -func_80071A54() % 10;
                state->velocity[i].primaryZ = func_80071A54() % 26 - 13;
                state->velocity[i].secondaryX = func_80071A54() % 26 - 13;
                state->velocity[i].secondaryY = -func_80071A54() % 10;
                state->velocity[i].secondaryZ = func_80071A54() % 26 - 13;
            }
            i++;
        } while (i < state->count);
    }
}

#undef ROOMLIB_SEED_MOTION_PARTICLES_FUNC
