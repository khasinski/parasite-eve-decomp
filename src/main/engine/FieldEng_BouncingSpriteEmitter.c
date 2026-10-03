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
