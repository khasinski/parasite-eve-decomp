#include "pe1/scene_e18_effects.h"

/* Emits up to 24 wave rings from random model joints, one every 12 to 19
 * frames, and freezes the model animation at frame 3 once it has played. */
int func_80193C24(int mode, SceneE18EmitterState *state) {
    switch (mode) {
    case 0:
        state->count = 0;
        state->delay = 0;
        return func_800CE560(D_800F33E0->pool, 24, 8, func_80193A58);
    case 1:
        if (state->count < 24) {
            SceneE18WaveParticle *particle;
            SceneE18Actor *actor;
            SceneE18Instance *instance;
            int random;
            int index;
            int count;
            int high;
            int tilt;

            if (--state->delay == -1) {
                particle = func_800CE610(D_800F33E0->pool);
                if (particle != 0) {
                    random = func_80071A54();
                    index = (random >> 2) % 38;
                    actor = D_800F32D0;
                    particle->position.x = actor->instance->transforms[index].t[0];
                    particle->position.y = actor->instance->transforms[index].t[1];
                    particle->position.z = actor->instance->transforms[index].t[2];
                    count = state->count;
                    particle->brightness = 128;
                    particle->amplitude = count * 2048 + 2048;
                    state->delay = (random & 7) + 12;
                    state->count++;
                    random = func_80071A54();
                    particle->controls[0] = (random & 0xFF) + 0x200;
                    high = (random >> 8) & 0xFF;
                    if (random & 2) {
                        tilt = high + 0x300;
                    } else {
                        tilt = high - 0x400;
                    }
                    particle->controls[1] = tilt;
                    particle->controls[2] = 0;
                    particle->controls[3] = 0;
                }
            }
            instance = D_800F32D0->instance;
            if (instance->animation.part.frame >= instance->frameLimit) {
                instance->animation.word = 0x30000;
            }
            break;
        }
        D_801941D0 = 1;
        return 2;
    case 2:
        D_800F3368.parameter00 = 64;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 64;
        D_800F3368.extent_y = 64;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
