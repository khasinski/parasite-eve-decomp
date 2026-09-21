#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/random.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
int func_800DD380(int mode, RenderConvergingSprite *state)
{
    RenderColor color __attribute__((aligned(4)));
    GteRotation rotation;
    GteShortVector position;
    int scale, weight, palette;
    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2) return 1;
        break;
    case 2:
        scale = 0;
        switch (state->stage) {
        case 0:
            weight = (state->timer * 4096) / 52;
            LoadAverageShort12(&state->position, &D_800E2234, 4096-weight, weight, &position);
            position.x += rsin(state->phase + D_800E27EC*160) / 16;
            state->position.y -= 2;
            func_800CF3AC(D_800E1FEC, &color, (D_800E27EC << 6) / 52);
            scale = rsin(D_800E27EC*128) / 2 + 4096;
            if (state->timer >= 52) {
                state->stage = 1;
                state->timer = 0;
                state->bursts = 0;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((D_800E27EC << 11) / 12);
            weight = scale / 32;
            /* The target writes all four packed color bytes in one word. */
            *(unsigned *)&color = (unsigned)weight | ((unsigned)weight << 8) | ((rand() & 15) << 16);
            if (state->timer >= 12) {
                state->bursts++;
                if (state->bursts < 4) {
                    state->position.x = D_800E2234.x;
                    state->position.y = D_800E2234.y;
                    state->position.z = D_800E2234.z;
                    state->position.x += rand() % 400 - 200;
                    state->position.y += rand() % 400 - 200;
                    state->position.z += rand() % 400 - 200;
                    state->timer = 0;
                } else state->stage = 2;
            }
            break;
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 12;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428) palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 128, GetClut(32,palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800DD76C(int mode, RenderConvergingEmitter *state)
{
    RenderConvergingSprite *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.y -= 550;
        return func_800CE560(D_800F33E0->end, 16, 24, (FieldAnimTaskCallback)func_800DD380);
    case 1:
        if (D_800E27EC < 49 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->position.x = state->position.x + rand() % 700 - 350;
                particle->position.y = state->position.y;
                particle->position.z = state->position.z + rand() % 700 - 350;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (D_800E27EC >= 140) return 1;
        break;
    case 2:
        D_800E2234.x = state->target.x;
        D_800E2234.y = state->target.y;
        D_800E2234.z = state->target.z;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
