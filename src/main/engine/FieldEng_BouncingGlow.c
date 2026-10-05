#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"

int func_800D6C58(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22D0;
    int scale, intensity, palette;
    unsigned int frame, angle;
    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 31 / 32;
        state->velocity_z = state->velocity_z * 31 / 32;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = D_800E27EC;
        intensity = 128;
        angle = state->angle;
        rotation.z = (angle << 8) + (frame << 7);
        palette = D_800E1204[D_800F336C];
        scale = (state->angle & 0x7FF) + 0x400;
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 0xBC,
            GetClut(128, palette), 255, intensity, &color);
        break;
    }
    return 0;
}

#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/random.h"
#include "pe1/gte.h"

int func_800D6E3C(int mode, RenderPhaseEmitter *state) {
    RenderBouncingSprite *particle;
    GteShortVector position;
    int count;
    int amplitude;

    switch (mode) {
    case 0:
        state->phase = rand();
        return func_800CE560(D_800F33E0->end, 16, 24,
                             (FieldAnimTaskCallback)func_800D6C58);
    case 1:
        if (D_800E27EC < 8) {
            for (count = 0; count < 3; count++) {
                particle = func_800CE610(D_800F33E0->end);
                if (particle != 0) {
                    position.x = D_800F32D0->actor->render_object.target_x;
                    position.y = D_800F32D0->actor->render_object.target_y;
                    position.z = D_800F32D0->actor->render_object.target_z;
                    particle->x = position.x;
                    particle->y = position.y;
                    particle->z = position.z;
                    state->phase += 0x955 + (rand() & 31);
                    amplitude = (rand() & 127) + 70;
                    particle->velocity_x = rsin(state->phase) * amplitude / 4096;
                    particle->velocity_z = rcos(state->phase) * amplitude / 4096;
                    particle->velocity_y = -(rand() & 31) - 36;
                    particle->duration = (rand() & 7) + 26;
                    particle->angle = rand();
                }
            }
        }
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
