#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

/* Rising effect emitter at the actor target: plays sound 0x487, opens a pool
 * of rising sprites (func_800D5CE4), scatters one per frame for eight frames
 * and draws a slowly turning glow while the anchor climbs. */
int func_800D5EB4(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    RenderRisingEffect *effect;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    u16 clut;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        Asset_Find08Alt(0x487, 0, state->position.x, state->position.y,
                        state->position.z);
        return func_800CE560(D_800F33E0->end, 16, 8,
                             (FieldAnimTaskCallback)func_800D5CE4);
    case 1:
        if (D_800E27EC < 8) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->x = state->position.x + (rand() & 0x7F) - 64;
                effect->y = state->position.y + (rand() & 0xFF) - 128;
                effect->z = state->position.z + (rand() & 0x7F) - 64;
                speed = (rand() & 7) + 16;
                effect->vx = rsin(state->phase) * speed / 4096;
                effect->vz = rcos(state->phase) * speed / 4096;
                effect->vy = -(rand() & 7) - 12;
                effect->angle = rand();
                state->phase += 0x955;
            }
        }
        if (D_800E27EC >= 30)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        state->position.y -= 4;
        intensity = rcos((D_800E27EC << 10) / 30) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -0x200;
        rotation.flags = 0;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        scale = 0x2400;
        if (kind == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(32, palette);
        func_800CEE20(&state->position, &rotation, scale, scale, 0x6C, clut,
                      1, intensity, 0);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        break;
    }
    return 0;
}
