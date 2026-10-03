/* MASPSX_FLAGS: --expand-div */
#include "pe1/scene_e18_effects.h"
#include "pe1/gte.h"

/* Every fourth frame spawns a rising particle at a random model joint,
 * pushed out by a random horizontal offset that grows with the spawn
 * count, until the script raises the stop flag. */
int func_801935F0(int mode, SceneE18EmitterState *state) {
    switch (mode) {
    case 0: {
        SceneE18Instance *instance = D_800F32D0->instance;
        GteMatrix *matrix = instance->transforms;

        func_8006DCE4(0x546, instance->owner->asset, (s16)matrix->t[0],
                      (s16)matrix->t[1], (s16)matrix->t[2]);
        state->count = 0;
        state->delay = 60;
        D_801941D0 = 0;
        return func_800CE560(D_800F33E0->pool, 12, 10, func_80193488);
    }
    case 1: {
        SceneE18RisingParticle *particle;
        GteShortVector offset;
        GteMatrix rotation;
        int random;
        int halfSize;
        int cosine;
        int sine;
        int count;
        int divisor;

        if (D_801941D0) return 2;
        if (state->delay-- > 0) break;
        particle = func_800CE610(D_800F33E0->pool);
        if (particle == 0) break;
        count = state->count;
        particle->size = (count << 5) + 0x800;
        particle->position.x = D_800F32D0->instance->transforms[func_80052B2C() % 38].t[0];
        particle->position.y = D_800F32D0->instance->transforms[func_80052B2C() % 38].t[1];
        particle->position.z = D_800F32D0->instance->transforms[func_80052B2C() % 38].t[2];
        random = func_80052B2C();
        particle->position.y -= ((random & 0x7F) * particle->size) >> 13;
        halfSize = particle->size >> 1;
        offset.x = 0;
        random = func_80071A54();
        divisor = 96;
        offset.z = ((random % divisor + 64) * halfSize) >> 12;
        cosine = rcos(func_80071A54());
        sine = rsin(func_80071A54());
        rotation.m[0][2] = sine;
        rotation.m[2][0] = -sine;
        rotation.m[1][1] = 0x1000;
        rotation.m[0][0] = cosine;
        rotation.m[2][2] = cosine;
        rotation.t[2] = 0;
        rotation.t[1] = 0;
        rotation.t[0] = 0;
        rotation.m[2][1] = 0;
        rotation.m[1][2] = 0;
        rotation.m[1][0] = 0;
        rotation.m[0][1] = 0;
        gte_ldrotmatrix(&rotation);
        gte_ldtransmatrix(&rotation);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        gte_stsv(&offset);
        particle->position.x += offset.x;
        particle->position.z += offset.z;
        particle->position.pad = 0;
        state->delay = 4;
        state->count++;
        func_8006DCE4(0x5FD, D_800F32D0->instance->owner->asset,
                      particle->position.x, particle->position.y,
                      particle->position.z);
        break;
    }
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    return 0;
}
