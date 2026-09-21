/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"

int FieldEng_CosineEffect(int mode, RenderCosineEffect *effect) {
    u32 color;

    switch (mode) {
    case 1:
        effect->x = (effect->amplitude
            * rcos((D_800E27EC << 10) / effect->duration)) / 4096;
        effect->y += 16;
        if (D_800E27EC >= effect->duration)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1C04, &color, D_800E27EC);
        func_800D27FC(effect->x, effect->y, &color, 128, 1);
        break;
    }
    return 0;
}

int func_800DACA4(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor first = D_800C22E8;
    RenderColor second = D_800C22EC;
    RenderCosineEffect *particle;
    int scale, intensity;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 8, 24, (FieldAnimTaskCallback)FieldEng_CosineEffect);
    case 1:
        if (D_800E27EC < 51) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->amplitude = (rand() & 255) + 500;
                particle->duration = (rand() & 3) + 22;
                particle->y = state->phase;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (D_800E27EC >= 80) return 1;
        break;
    case 2:
        if (D_800E27EC <= 80) {
            D_800F3368.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = (D_800E27EC << 12) / 80;
            intensity = rsin((D_800E27EC << 11) / 80) / 32;
            func_800D004C(&position, 800, 800, 16, 0, scale, scale, &first, 0, intensity, 1);
            func_800D004C(&position, 250, 250, 8, 0, scale, scale, &second, 0, intensity, 1);
            func_800D0728(&position, 590, 670, 16, 0, scale, scale, 0, &first, intensity, 3);
        }
        D_800F3368.depth = 64;
        FieldEng_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}
